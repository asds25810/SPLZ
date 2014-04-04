#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <io.h>
#include <math.h>
#include "Graph.h"

#define N 500000
void test();
void RLE();
void sim_test();
int Encode(unsigned char *dictionary,unsigned char *source,unsigned char *dest,int length,int *sum_length,int *sum_location);
void Decode(unsigned char *dictionary,unsigned char *dest,unsigned char *source,int length_source);
int _main()
{
	LARGE_INTEGER begin,end,lv;
	double totaltime,secondsPerTick;
	QueryPerformanceFrequency(&lv);
	secondsPerTick=1000000.0/lv.QuadPart;
	srand(time(0));
	//for(int i=0;i<10;i++)
	//	printf("%d\n",rand()%10000);
	//test();
	//return 0;

	Graph graph("E:\\GraphData\\USA-road-d.NW.gr");//载入图数据
	graph.LoadCoordinate("E:\\GraphData\\USA-road-d.NW.co");

	//graph.Segment3((int)sqrt((double)graph.size)*2,30);
	//printf("结点到中心的实际距离和为：%f\n",graph.evaluate());
	//return 0;

	graph.LoadRegion("E:\\GraphData\\regions_Kmeans_2000_step5_imba.txt");
	//graph.Compress("I:\\record_2000_step5_imba\\");
	//return 0;

	//以下是解压测试
	QueryPerformanceCounter( &begin );
	graph.Decompress_prepare("I:\\NW_Kmeans_2000_step5_imba\\",true);
	QueryPerformanceCounter( &end );
	totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
	printf("解压准备时间=%fus\n",totaltime);

	int s=1;
	int n_steps=0;
	int sum_steps=0;
	double sum=0;
	unsigned char *buf[2];
	buf[0]=new unsigned char[graph.size];
	buf[1]=new unsigned char[graph.size];

	unsigned char *path=new unsigned char[graph.size];
	int *dist=new int[graph.size];
	int *label=new int[graph.size];
	unsigned char *result=0;
	/*
	for(int i=0;i<N;i++)
	{
		s=rand()%(graph.size-1)+1;
		QueryPerformanceCounter( &begin );
		graph.Dijkstra(s,path,label,dist);
		QueryPerformanceCounter( &end );
		totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
		sum+=totaltime;
	}printf("平均时间%fus\n",sum/N);return 0;
	*/
	double r[1000]={0};
	for(int i=0;i<N;i++)
	{
		s=rand()%(graph.size-1)+1;
		QueryPerformanceCounter( &begin );
		result=graph.regions[graph.nodes[s].id_region].data->GetRecord(buf,graph.nodes[s].id_in_region,true,n_steps);
		QueryPerformanceCounter( &end );
		totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
		if(i>0.9*N)	sum+=totaltime;
		if(i>0.9*N) begin_to_record=true;
		sum_steps+=n_steps;
		if(i%(N/1000)==0)
		{
			r[i/(N/1000)]=totaltime;
			printf("%d\n",i);
		}
		/*
		graph.Dijkstra(s,path,label,dist);
		if(memcmp(result,path,graph.size)!=0)
		{
			printf("解码出错！！！！！！\n",i);
			FILE *fp=fopen("check.txt","w");
			for(int j=1;j<graph.size;j++)
				if(path[j]!=result[j])
					fprintf(fp,"%d %d %d\n",j,(int)path[j],(int)result[j]);
			fclose(fp);
			return 0;
		}*/
	}
	printf("平均解压时间%fus\n",sum/N);
	printf("读取数据时间%fus\n",global_time/N);
	printf("平均解压步数%.2f\n",sum_steps*1.0/N);

	FILE *fp=fopen("test.txt","w");
	for(int i=0;i<1000;i++)
		fprintf(fp,"%f\n",r[i]);
	fprintf(fp,"\n\n");
	for(int i=0;i<100;i++)
		fprintf(fp,"%f\n",r[i*10]);
	fclose(fp);
	return 0;
}



void test()
{
	Graph graph("E:\\GraphData\\USA-road-d.NW.gr");//载入图数据
	graph.LoadCoordinate("E:\\GraphData\\USA-road-d.NW.co");
	//graph.LoadRegion("E:\\GraphData\\regions_kmeans.txt");

	int sum_length[5]={0},sum_location[5]={0};
	unsigned char *path=new unsigned char[graph.size];
	unsigned char *path2=new unsigned char[graph.size];
	int *path3=new int[graph.size];
	int *path4=new int[graph.size];
	int *dist=new int[graph.size];
	int *dist2=new int[graph.size];
	int *label=new int[graph.size];
	unsigned char *dest=new unsigned char[graph.size*3];
	int count[1000]={0};
	double sim[1000]={0};
	double sum_dijkstra=0;
	double sum_encode=0;
	double sum_decode=0;
	int s=0;
	int ss=0;

	LARGE_INTEGER begin,end,lv;
	double totaltime,secondsPerTick;
	QueryPerformanceFrequency(&lv);
	secondsPerTick=1000000.0/lv.QuadPart;
	/*
	FILE *f=fopen("AstarTest.txt","w");
	int source[N]={0},target[N]={0},distance[N]={0};
	for(int i=0;i<N;i++)
	{
		source[i]=rand()%(graph.size-1)+1;
		target[i]=rand()%(graph.size-1)+1;
	}

	double r=0;
	printf("%d\n",0);
	double sum_time=0;
	int sum_space=0;
	for(int i=0;i<N;i++)
	{
		QueryPerformanceCounter( &begin );
		sum_space+=graph.Astar(source[i],target[i],path3,label,0);
		QueryPerformanceCounter( &end );
		totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
		sum_time+=totaltime;
		double d=CalculateDistance(graph.nodes[source[i]].x,graph.nodes[target[i]].x,graph.nodes[source[i]].y,graph.nodes[target[i]].y);
		r+=d/(label[target[i]]);
		distance[i]=label[target[i]];
	}
	fprintf(f,"%f\n",r/N);
	fprintf(f,"%.1f\t%10.0f\t%d\t%.3f\n",0.0,sum_time/N,sum_space/N,1.000);

	for(int k=10;k<=30;k++)
	{
		printf("%d\n",k);
		double sum_time=0;
		int sum_space=0;
		double sum_error=0;
		for(int i=0;i<N;i++)
		{
			QueryPerformanceCounter( &begin );
			sum_space+=graph.Astar(source[i],target[i],path3,label,k*0.1);
			QueryPerformanceCounter( &end );
			totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
			sum_time+=totaltime;
			sum_error+=label[target[i]]*1.0/distance[i];

		}
		fprintf(f,"%.1f\t%10.0f\t%d\t%.3f\n",k*0.1,sum_time/N,sum_space/N,sum_error/N);
	}
	fclose(f);
	return;
	*/
	for(int i=0;i<25;i++)
	{
		
		s=rand()%(graph.size-1)+1;

		graph.Dijkstra(s,path,label,dist);
		
		for(int j=0;j<40;j++)//graph.regions[i].size;j++)
		{
			printf("%d %d\n",i,j);
			int a=rand()%(graph.size-1)+1;
			while(dist[a]>100)
				a=rand()%(graph.size-1)+1;
			//if(dist[graph.regions[i].nodes[a]]<50)
			{
				QueryPerformanceCounter( &begin );
				graph.Dijkstra(a,path2,label,dist2);
				QueryPerformanceCounter( &end );
				totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
				sum_dijkstra+=totaltime;

				QueryPerformanceCounter( &begin );
				sim[dist[a]]+=(ss=Encode(path2,path,dest,graph.size,sum_length,sum_location))*1.0/graph.size;//cos_sim(path,path2,graph.size);
				QueryPerformanceCounter( &end );
				totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
				sum_encode+=totaltime;

				QueryPerformanceCounter( &begin );
				Decode(path2,path,dest,ss);
				QueryPerformanceCounter( &end );
				totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
				sum_decode+=totaltime;

				count[dist[a]]++;
			}
		}
	}
	printf("length: %d %d %d %d %d	%d\n",sum_length[0],sum_length[1],sum_length[2],sum_length[3],sum_length[4],sum_length[0]+sum_length[1]+sum_length[2]+sum_length[3]+sum_length[4]);
	printf("location: %d %d %d %d	%d\n",sum_location[0],sum_location[1],sum_location[2],sum_location[3],sum_location[0]+sum_location[1]+sum_location[2]+sum_location[3]);
	printf("Dijkstra时间%fus\n",sum_dijkstra/1000);
	printf("Encode时间%fus\n",sum_encode/1000);
	printf("Decode时间%fus\n",sum_decode/1000);

	FILE *fp=fopen("E:\\GraphData\\similarity.txt","w");
	for(int i=0;i<=100;i++)
	{
		if(count[i]!=0)
			sim[i]=sim[i]/count[i];
		fprintf(fp,"%d\t%d\t%f\n",i,count[i],sim[i]);
	}
	fclose(fp);
	return ;
}

#define WINDOW_SIZE_TEST 1000
int Encode(unsigned char *dictionary,unsigned char *source,unsigned char *dest,int length,int *sum_length,int *sum_location)
{
	int last_location=0;
 	int *fail=new int[length];
	KMP_CalculateFail(source,fail,length);//求KMP失配数组
	int source_index=0;//source数组的当前位置
	int size_current=0;
	while(source_index!=length)
	{
		int max_location=0,max_length=0;//最长匹配的位置和匹配长度
		int dictionary_index=(source_index-WINDOW_SIZE_TEST<=0?0:source_index-WINDOW_SIZE_TEST);//dictionary数组的当前位置
		int i=source_index;//i表示source中当前已经匹配到的位置
		while(dictionary_index<length //字典未到尾
			   && i<length)           //源数据未处理完
		{
			//开始匹配直到失配或者到达字典末尾或者到达源数据末尾，记录最长匹配的位置和匹配长度。
			while(dictionary_index<length && i<length && source[i]==dictionary[dictionary_index])
			{
				dictionary_index++;
				i++;
			}
			if(i-source_index>max_length)
			{
				max_location=dictionary_index-(i-source_index);
				max_length=i-source_index;
			}
			if(dictionary_index>=length || i>=length)//到达字典末尾或者到达源数据末尾
				break;
			if(max_length!=0&&dictionary_index>=(source_index+WINDOW_SIZE_TEST>length?length:source_index+WINDOW_SIZE_TEST))
			{
				break;
			}
			i=fail[i];//失配滑动
			if(source_index>i)
			{
				
					//dictionary_index+=i-source_index;
					//i=source_index;
				
				i=source_index;
				dictionary_index++;
			}				
		}
		unsigned char buf[4]={0};
		//输出压缩数据
		if(max_length==0)// 1111 0xxx    修改为 1111 xxxx
		{
			//printf("未能进行压缩！！！%d\n",(int)source[source_index]);
			dest[size_current]=(source[source_index]|0xf0);//&0xf7; 修改
			size_current++;
			source_index++;	
			sum_length[0]++;
			continue;
		}
		else if(max_length<=0x7f)// 0xxx xxxx
		{
			dest[size_current]=max_length&0x7f;
			size_current++;
			sum_length[1]++;
		}
		else if(max_length<=0x3fff)// 10xx xxxx xxxx xxxx
		{
			dest[size_current]=((max_length&0xff00)>>8)|0x80;
			dest[size_current+1]=max_length&0xff;
			size_current+=2;
			sum_length[2]++;
		}
		else if(max_length<=0x1fffff)// 110x xxxx xxxx xxxx xxxx xxxx
		{
			dest[size_current]=((max_length&0xff0000)>>16)|0xc0;
			dest[size_current+1]=(max_length&0xff00)>>8;
			dest[size_current+2]=max_length&0xff;
			size_current+=3;
			sum_length[3]++;
		}
		else if(max_length<=0x0fffffff)// 1110 xxxx xxxx xxxx xxxx xxxx xxxx xxxx
		{
			dest[size_current]=((max_length&0xff000000)>>24)|0xe0;
			dest[size_current+1]=(max_length&0xff0000)>>16;
			dest[size_current+2]=(max_length&0xff00)>>8;
			dest[size_current+3]=max_length&0xff;
			size_current+=4;
			sum_length[4]++;
		}
		else
		{
			printf("length超出长度范围！！！\n");
			return 0;
		}

		max_location=max_location-last_location;//不是记录实际值，而是记录差值
		last_location=last_location+max_location;
		bool flag=(max_location<0);
		max_location=abs(max_location);
		
		if(max_location<=0x3f)//&&max_location>=-0x3f)// 0xxx xxxx
		{
			dest[size_current]=max_location&0x7f;
			if(flag)
				dest[size_current]=dest[size_current]|0x40;//01xx xxxx
			size_current++;
			sum_location[0]++;
		}
		else if(max_location<=0x1fff)//&&max_location>=-0x1fff)// 10xx xxxx xxxx xxxx
		{
			dest[size_current]=((max_location&0xff00)>>8)|0x80;
			dest[size_current+1]=max_location&0xff;
			if(flag)
				dest[size_current]=dest[size_current]|0x20;//101x xxxx
			size_current+=2;
			sum_location[1]++;
		}
		else if(max_location<=0x0fffff)//&&max_location>=-0x0fffff)// 110x xxxx xxxx xxxx xxxx xxxx
		{
			dest[size_current]=((max_location&0xff0000)>>16)|0xc0;
			dest[size_current+1]=(max_location&0xff00)>>8;
			dest[size_current+2]=max_location&0xff;
			if(flag)
				dest[size_current]=dest[size_current]|0x10;//1101 xxxx
			size_current+=3;
			sum_location[2]++;
		}
		else if(max_location<=0x07ffffff)//&&max_location>=-0x07ffffff)// 1110 xxxx xxxx xxxx xxxx xxxx xxxx xxxx
		{
			dest[size_current]=((max_location&0xff000000)>>24)|0xe0;
			dest[size_current+1]=(max_location&0xff0000)>>16;
			dest[size_current+2]=(max_location&0xff00)>>8;
			dest[size_current+3]=max_location&0xff;
			if(flag)
				dest[size_current]=dest[size_current]|0x08;//1110 1xxx
			size_current+=4;
			sum_location[3]++;
		}		
		else
		{
			printf("location超出长度范围！！！\n");
			return 0;
		}		//dest[size_current]=max_location;
		//dest[size_current+1]=max_length;
		//size_current+=2;
		source_index+=max_length;
		
		//if(size_current>=size_max)
		//{
		//	data=(int*)realloc(data,size_current*2*sizeof(int));
		//	size_max=size_current*2;
		//}
		
	}
	delete fail;
	return size_current;
}

inline void Decode(unsigned char *dictionary,unsigned char *dest,unsigned char *source,int length_source)
{
	//FILE *fp=fopen("out2.txt","w");
	int p=0;
	int length=0,location=0,step=0,last_location=0;
	unsigned char *buf;
	for(int i=0;i<length_source;i=i+step)
	{
		buf=&source[i];
		step=0;
		//if((*buf&0xf8)==0xf0)//未压缩
		if((*buf&0xf0)==0xf0)//未压缩   修改方案
		{
			*(dest+p)=*buf&0x0f;//*buf&0x07;	修改方案
			p+=1;
			step++;
			continue;
		}
		else
		{
			if((*buf&0x80)==0)//1字节
			{
				length=*buf;
				step++;
			}
			else if((*buf&0xc0)==0x80)//2字节
			{
				length=((buf[0]&0x3f)<<8)+buf[1];//((buf[0]&0x3f)*256)+buf[1]
				step+=2;
			}
			else if((*buf&0xe0)==0xc0)
			{
				length=((buf[0]&0x1f)<<16)+(buf[1]<<8)+buf[2];//((buf[0]&0x1f)*256*256)+(buf[1]*256)+buf[2]
				step+=3;
			}
			else if((*buf&0xf0)==0xe0)
			{
				length=((buf[0]&0x0f)<<24)+(buf[1]<<16)+(buf[2]<<8)+buf[3];
				step+=4;
			}
			else
				printf("解压出错！！！\n");
			buf+=step;
			if((*buf&0x80)==0)//1字节
			{
				if((*buf&0x40))
					location=-(*buf&0x3f);
				else
					location=(*buf&0x3f);
				step++;
			}
			else if((*buf&0xc0)==0x80)//2字节
			{
				if((*buf&0x20))
					location=-(((buf[0]&0x1f)<<8)+buf[1]);
				else
					location=((buf[0]&0x1f)<<8)+buf[1];//((buf[0]&0x3f)*256)+buf[1]
				step+=2;
			}
			else if((*buf&0xe0)==0xc0)
			{
				if((*buf&0x10))
					location=-(((buf[0]&0x0f)<<16)+(buf[1]<<8)+buf[2]);
				else
					location=((buf[0]&0x0f)<<16)+(buf[1]<<8)+buf[2];//((buf[0]&0x1f)*256*256)+(buf[1]*256)+buf[2]
				step+=3;
			}
			else if((*buf&0xf0)==0xe0)
			{
				if((*buf&0x08))
					location=-(((buf[0]&0x07)<<24)+(buf[1]<<16)+(buf[2]<<8)+buf[3]);
				else
					location=((buf[0]&0x07)<<24)+(buf[1]<<16)+(buf[2]<<8)+buf[3];
				step+=4;
			}
			else
			{
				printf("解压出错！！！\n");
				//fclose(fp);
			}
			//fprintf(fp,"%d %d %d\n",length,last_location,location);
			location=location+last_location;
			last_location=location;
			memcpy(dest+p,dictionary+location,length);
		}
		p+=length;
	}
}



void sim_test()
{
	Graph graph("E:\\GraphData\\USA-road-d.NY.gr");//载入图数据
	graph.LoadCoordinate("E:\\GraphData\\USA-road-d.NY.co");

	unsigned char *path=new unsigned char[graph.size];
	unsigned char *path2=new unsigned char[graph.size];
	int *dist=new int[graph.size];
	int *dist2=new int[graph.size];
	int *label=new int[graph.size];
	unsigned char *dest=new unsigned char[graph.size];
	int count[1000]={0};
	double sim[1000]={0};
	
	for(int i=0;i<10000;i++)
	{
		int s=rand()%(graph.size-1)+1;
		
		int t=rand()%(graph.size-1)+1;

		graph.Dijkstra(s,path,label,dist);
		while(dist[t]>100)
				t=rand()%(graph.size-1)+1;
		graph.Dijkstra(t,path2,label,dist2);

		int same=0;
		for(int j=1;j<graph.size;j++)
			if(path[j]==path2[j])
				same++;
		sim[dist[t]]+=same*1.0/(graph.size-1);
		count[dist[t]]++;
		printf("%d\n",i);
	}
	FILE *fp=fopen("E:\\GraphData\\similarity.txt","w");
	for(int i=0;i<=100;i++)
	{
		if(count[i]!=0)
			sim[i]=sim[i]/count[i];
		fprintf(fp,"%d\t%d\t%f\n",i,count[i],sim[i]);
	}
	fclose(fp);
}

void RLE()
{
	Graph graph("E:\\GraphData\\USA-road-d.CTR.gr");//载入图数据
	graph.LoadCoordinate("E:\\GraphData\\USA-road-d.CTR.co");

	unsigned char *path=new unsigned char[graph.size+1];
	unsigned char *dest=new unsigned char[graph.size*10];
	int *dist=new int[graph.size+1];
	int *label=new int[graph.size];

	double sum=0;
	for(int i=0;i<200;i++)
	{
		int s=rand()%(graph.size-1)+1;
		graph.Dijkstra(s,path,label,dist);
		int index=0;
		int size_processed=0;
		dist[graph.size]=0x7F;
		while(index!=graph.size)
		{
			unsigned char begin=index;
			do
			{
				index++;
			}
			while(dist[index]==dist[index-1]);
			
			if(index-begin==1)
			{
				dest[size_processed]=dist[index-1]+0x80;
				size_processed++;
			}
			else
			{
				dest[size_processed]=dist[index-1];
				size_processed++;
				int length=index-begin;
				if(length<=0x7f)// 0xxx xxxx
				{
					dest[size_processed]=length&0x7f;
					size_processed++;
				}
				else if(length<=0x3fff)// 10xx xxxx xxxx xxxx
				{
					dest[size_processed]=((length&0xff00)>>8)|0x80;
					dest[size_processed+1]=length&0xff;
					size_processed+=2;
				}
				else if(length<=0x1fffff)// 110x xxxx xxxx xxxx xxxx xxxx
				{
					dest[size_processed]=((length&0xff0000)>>16)|0xc0;
					dest[size_processed+1]=(length&0xff00)>>8;
					dest[size_processed+2]=length&0xff;
					size_processed+=3;
				}
				else if(length<=0x0fffffff)// 1110 xxxx xxxx xxxx xxxx xxxx xxxx xxxx
				{
					dest[size_processed]=((length&0xff000000)>>24)|0xe0;
					dest[size_processed+1]=(length&0xff0000)>>16;
					dest[size_processed+2]=(length&0xff00)>>8;
					dest[size_processed+3]=length&0xff;
					size_processed+=4;
				}
				else
				{
					printf("length超出长度范围！！！\n");
					return ;
				}
			}
		}
		sum+=size_processed;
		printf("%d  单条记录压缩比:%.2f\n",i,graph.size*1.0/size_processed);
	}
	printf("平均压缩比%.2f\n",200.0*graph.size/sum);
}