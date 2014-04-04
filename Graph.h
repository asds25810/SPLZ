

#define INFINITY 2147483647
//#define INFINITY_DOUBLE  
#include "PriorityQueue.h"
#include "head.h"
#include <sys/io.h>
#include <hash_map>
//using namespace stdext;
Graph::Graph()
{
	size=0;
	nodes=NULL;
	size_current=0;
	count_current=0;
}
Graph::Graph(char *filepath)
{
	LoadGraph(filepath);
	size_current=0;
	count_current=0;
}
void Graph::LoadGraph(char *filepath)//载入图信息
{
	//int n_arc;
	int *maxdegree;
	double sum_arc=0;
	FILE *fp=fopen(filepath,"r");
	if(!fp)
	{
		printf("graph data load fail！\n");
		printf("%s\n",filepath);
	}
	char buf[200]={0};
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fscanf(fp,"p sp %d%d\n",&size,&n_arc);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	printf("%d\n",size);
	size++;
	nodes=(vertex*)malloc(sizeof(vertex)*(size));
	maxdegree=new int[size];
	memset(maxdegree,0,sizeof(int)*size);
	memset(nodes,0,sizeof(vertex)*size);

	int from,to,weight;
	for(int i=0;i<n_arc;i++)
	{
		char c=fgetc(fp);
		c=fgetc(fp);
		fscanf(fp,"%d%d%d",&from,&to,&weight);
		sum_arc+=weight;
		if(nodes[from].arcs==NULL)
		{
			nodes[from].arcs=(arc*)malloc(sizeof(arc)*5);
			maxdegree[from]=5;
		}
		else if(nodes[from].degree==maxdegree[from])
		{
			maxdegree[from]++;
			nodes[from].arcs=(arc*)realloc(nodes[from].arcs,maxdegree[from]*sizeof(arc));
		}
		nodes[from].id=from;
		//nodes[from].id_region=-1;
		nodes[from].arcs[nodes[from].degree].id_to=to;
		nodes[from].arcs[nodes[from].degree].weight=weight;
		nodes[from].arcs[nodes[from].degree].importance=0;
		nodes[from].degree++;
	}
	//delete(maxdegree);
	fclose(fp);


	int max=0;
	for(int i=1;i<size;i++)
		if(maxdegree[i]>max)
			max=maxdegree[i];
	printf("载入完毕\n");
	//printf("最大度数：%d\n",max);
	//printf("平均边长：%f\n",sum_arc/n_arc);

	//岔路分支数大于8时，进行分解
	delete maxdegree;
	size_current=0;
	count_current=0;
}

void Graph::LoadCoordinate(char *filename)//载入坐标信息
{
	FILE *fp=fopen(filename,"r");
	char buf[200]={0};
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	fgets(buf,200,fp);
	int x,y,v;
	this->x_east=-INFINITY;
	this->x_west=INFINITY;
	this->y_north=-INFINITY;
	this->y_south=INFINITY;
	for(int i=1;i<size;i++)
	{
		char c=fgetc(fp);
		c=fgetc(fp);
		fscanf(fp,"%d%d%d",&v,&x,&y);
		nodes[v].x=x;
		nodes[v].y=y;
		if(x>x_east)
			x_east=x;
		if(x<x_west)
			x_west=x;
		if(y>y_north)
			y_north=y;
		if(y<y_south)
			y_south=y;
	}
	fclose(fp);
}

void Graph::Dijkstra(int source,int *path,int *label)//普通dijkstra
{
	int id=source;
	int *hash=(int*)calloc(size,sizeof(int));
	bool *mark=(bool*)calloc(size,sizeof(bool));
	for (int i = 0; i < size; i++)
    {
		//mark[i]=false;
        //path[i] = -1;
        hash[i] = -1;
		//label[i]=INFINITY;
    }
	memset(label,0,size*sizeof(int));
	PriorityQueue Q(10000,label,hash);
	
	path[0]=0;
	path[source]=0;//-1
    label[source] = 0;
    do
    {     
		for (int i=0;i<nodes[id].degree;i++)
		{
			int id_to=nodes[id].arcs[i].id_to;
			if (!mark[id_to] && (label[id_to]==0||label[id] + nodes[id].arcs[i].weight < label[id_to]))
            {
                label[id_to] = label[id] + nodes[id].arcs[i].weight;
				if (hash[id_to] == -1)
                {
                    Q.Push(id_to);
                }
                else
                {
                    Q.Adjust(hash[id_to]);
                }
				/*for(int j=0;j<nodes[id_to].degree;j++)
					if(nodes[id_to].arcs[j].id_to==id)
					{
						path[id_to] = j;
						break;
					}*/
				path[id_to] = id;//i;//
            }
		}
		mark[id]=true;
        id = Q.Top();
        Q.Pop();
    }
	while(id!=-1);
	//path[source]=label[target];
	free(hash);
	free(mark);
}

void Graph::Dijkstra(int source,unsigned char *path,int *label,int *dist)//计边数的dijkstra
{
	if(source==0)
		memset(path,0,size);
	else
	{
		int id=source;
		int *hash=(int*)calloc(size,sizeof(int));
		bool *mark=(bool*)calloc(size,sizeof(bool));
		for (int i = 0; i < size; i++)
		{
			//mark[i]=false;
			//path[i] = -1;
			hash[i] = -1;
			//label[i]=INFINITY;
		}
		memset(label,0,size*sizeof(int));
		PriorityQueue Q(10000,label,hash);
	
		path[0]=0;
		dist[0]=0;
		path[source]=0;//-1
		label[source] = 0;
		dist[source]=0;

		//for(int i=0;i<nodes[id].degree;i++)
		//	dist[nodes[id].arcs[i].id_to]=i;

		do
		{     
			for (int i=0;i<nodes[id].degree;i++)
			{
				int id_to=nodes[id].arcs[i].id_to;
				if (!mark[id_to] && (label[id_to]==0||label[id] + nodes[id].arcs[i].weight < label[id_to]))
				{
					label[id_to] = label[id] + nodes[id].arcs[i].weight;
					dist[id_to] = dist[id] + 1;
					//if(id!=source)
					//	dist[id_to]=dist[id];
					if (hash[id_to] == -1)
					{
						Q.Push(id_to);
					}
					else
					{
						Q.Adjust(hash[id_to]);
					}
					for(int j=0;j<nodes[id_to].degree;j++)
						if(nodes[id_to].arcs[j].id_to==id)
						{
							path[id_to] = j;
							break;
						}
					//path[id_to] = i;//id;
				}
			}
			mark[id]=true;
			id = Q.Top();
			Q.Pop();
		}
		while(id!=-1);
		//path[source]=label[target];
		free(hash);
		free(mark);
	}
}


inline double CalculateDistance(double x1,double x2,double y1,double y2)
{
	double result=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
	return result;
	//return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}
/*
struct str_hash //: hash_compare<unsigned char*>
{
	 enum   
	 {   //   parameters   for   hash   table   
		  bucket_size   =   4,   //   0   <   bucket_size   
		  min_buckets   =   8  //   min_buckets   =   2   ^^   N,   0   <   N   
	 };   
    size_t operator()(unsigned char *ss) const
    {
           return size_t(ss[0]*0x1000000+ss[1]*0x10000+ss[2]*0x100+ss[3]*0x1);
    }
	bool operator()(unsigned char *s1,unsigned char *s2)const
	{
		return  s1[0]==s2[0]&&s1[1]==s2[1]&&s1[2]==s2[2]&&s1[3]==s2[3];
	}
};
typedef hash_map<unsigned char*,vector<int>*,str_hash> fast_table;
fast_table* creat_hash(unsigned char *dic,int length)
{
	fast_table *hm=new fast_table;
	for(int i=0;i+4<length;i++)
	{
		if(hm->find(&dic[i])!=hm->end())
			(*hm)[&dic[i]]=new vector<int>;
		((*hm)[&dic[i]])->push_back(i);
	}

	return hm;
}
*/


void Get_Order(vector<int> *order,int now,int *tree,bool *mark)
{
	mark[now]=true;
	if(!mark[tree[now]])
		Get_Order(order,tree[now],tree,mark);		
	order->push_back(now);
}
void Get_Order_BFS(vector<int> *order,int *tree,int size)
{
	bool *mark=new bool[size];
	memset(mark,0,sizeof(bool)*size);
	queue<int> Q;
	vector<int> *children=new vector<int>[size];
	memset(children,0,sizeof(vector<int>)*size);
	//将反向的依赖树转换为正向
	for(int i=0;i<size;i++)
	{
		children[tree[i]].push_back(i);
		//printf("%d\n",i);
	}
	//开始BFS
	Q.push(0);
	while(!Q.empty())
	{
		int now=Q.front();
		Q.pop();
		if(!mark[now])
		{
			mark[now]=true;
			order->push_back(now);
			for(int i=0;i<children[now].size();i++)
				Q.push(children[now][i]);
		}
	}
	delete mark;
	//delete children;
}

void Graph::Segment(int k)
{
	n_regions=k;
	//读取排序数据
	FILE *fp=fopen("E:\\GraphData\\importance.hcn","rb");
	for(int i=0;i<size;i++)
		fread(&nodes[i].importance,sizeof(int),1,fp);
	fclose(fp);
	//选择中心
	vector<vertex> corner;
	for(int i=1;i<size;i++)
		corner.push_back(nodes[i]);
	sort(corner.begin(),corner.end());
	//是否需要输出中心信息到文件？
	//求出每个结点所属区域
	unsigned char *path=new unsigned char[size];            
	int *dist=new int[size];
	int *label=new int[size];//距离以跳数为准还是长度为准？
	unsigned char *dest=new unsigned char[size];
	int *buf=new int[size];//每个结点目前的最近距离
	for(int i=0;i<size;i++)
		buf[i]=0x7fffffff;
	for(int i=0;i<n_regions;i++)
	{
		printf("划分区域%d\n",i);
		Dijkstra(corner[i].id,path,label,dist);
		for(int j=1;j<size;j++)
			if(buf[j]>label[j])
			{
				nodes[j].id_region=i;
				buf[j]=label[j];
			}
	}
	regions=new region[k];
	for(int i=1;i<size;i++)//区域内部初步编号
	{
		nodes[i].id_in_region=regions[nodes[i].id_region].nodes.size();
		regions[nodes[i].id_region].nodes.push_back(i);
	}
	for(int i=0;i<k;i++)	
	{
		printf("设置代表元%d\n",i);
		regions[i].size=regions[i].nodes.size();
		//把代表元换到第一个
		int tmp=regions[i].nodes[0];
		regions[i].nodes[0]=regions[i].nodes[nodes[corner[i].id].id_in_region];
		regions[i].nodes[nodes[corner[i].id].id_in_region]=tmp;
		nodes[tmp].id_in_region=nodes[corner[i].id].id_in_region;
		nodes[regions[i].nodes[0]].id_in_region=0;
	}

	Reorder("E:\\GraphData\\regions_CH.txt");
	/*fp=fopen("E:\\GraphData\\regions_CH.txt","w");
	fprintf(fp,"%d\n",n_regions);
	for(int i=0;i<n_regions;i++)
	{
		fprintf(fp,"%d",regions[i].size);
		for(int j=0;j<regions[i].size;j++)
			fprintf(fp," %d",regions[i].nodes[j]);
		fprintf(fp,"\n");
	}
	fclose(fp);*/
}
void Graph::Reorder(char *filename)
{
	unsigned char *path=new unsigned char[size];            
	int *dist=new int[size];
	int *label=new int[size];//距离以跳数为准还是长度为准？
	unsigned char *dest=new unsigned char[size];
	vector<int> order;
	FILE *fp=fopen(filename,"w");
	fprintf(fp,"%d\n",n_regions);
	for(int i=0;i<n_regions;i++)
	{
		printf("重新编号%d\n",i);
		order.clear();
		//求根字典
		Dijkstra(regions[i].nodes[0],path,label,dist);
		//求解压依赖树
		int *tree=new int[regions[i].size];
		//regions[i].data->depend_tree=tree;
		tree[0]=0;
		for(int j=1;j<regions[i].size;j++)
		{
			tree[j]=regions[i].nodes[j];
			int region=i;
			for(int k=0;k<STEP&&tree[j]!=regions[i].nodes[0];k++)
			{
				tree[j]=nodes[tree[j]].arcs[path[tree[j]]].id_to;
			}
			int c=0;
			while(nodes[tree[j]].id_region!=region)
			{
				printf("出界！！！！超出预定距离%d\n",++c);
				tree[j]=nodes[tree[j]].arcs[path[tree[j]]].id_to;
			}
			tree[j]=nodes[tree[j]].id_in_region;
		}
		//区域内宽度优先重排
		//宽度优先序
		Get_Order_BFS(&order,tree,regions[i].size);

		if(order.size()!=regions[i].size)
		{
			printf("出错！！！！！！");
			return;
		}
		//排好之后输出到文件
		
		fprintf(fp,"%d",regions[i].size);
		for(int l=0;l<regions[i].size;l++)
		{
			fprintf(fp," %d",regions[i].nodes[order[l]]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}
		
void Graph::LoadRegion(char *filename)
{
	FILE *fp=fopen(filename,"r");
	fscanf(fp,"%d",&n_regions);
	regions=new region[n_regions];
	memset(regions,0,sizeof(region)*n_regions);
	for(int i=0;i<n_regions;i++)
	{
		//printf("载入区域信息%d\n",i);
		fscanf(fp,"%d",&regions[i].size);
		regions[i].nodes.resize(regions[i].size);
		for(int j=0;j<regions[i].size;j++)
		{
			fscanf(fp,"%d",&regions[i].nodes[j]);
			nodes[regions[i].nodes[j]].id_region=i;
			nodes[regions[i].nodes[j]].id_in_region=j;
		}
		
		//用于存储压缩数据的空间
		regions[i].data=new DataBlock;
		regions[i].data->id=i;
		//regions[i].data->nodes=nodes;
		regions[i].data->init(size,regions[i].size,size/2000*2+100);
		regions[i].data->depend_tree=new int[regions[i].size];
		//载入依赖树
		for(int j=0;j<regions[i].size;j++)
		{
			fscanf(fp,"%d",&regions[i].data->depend_tree[j]);
		}
	}
	fclose(fp);

	/*int sum=0;
	for(int i=0;i<n_regions;i++)
		sum+=regions[i].size;
	printf("%d %d\n",size,sum);*/
}

void Graph::Get_Importance()
{
	char inputpath[100]="I:\\AllData1\\";
	char index[20]={0};
	char filename[100]={0};
	unsigned char *path=new unsigned char[size];
	int *dist=new int[size];
	int *label=new int[size];
	int i;
	for(i=0;i<size/1000;i++)
	{
		//itoa(i,index,10);
		sprintf(index,"%d",i);
		strcpy(filename,inputpath);
		strcat(filename,"sptree");
		strcat(filename,index);
		FILE *fp_tree=fopen(filename,"rb");
		for(int j=0;j<1000;j++)
		{
			//Dijkstra(i*1000+j,path,label,dist);
			fread(path,sizeof(unsigned char)*size,1,fp_tree);
			for(int k=1;k<size;k++)
			{
				nodes[k].arcs[path[k]].importance++;
				nodes[nodes[k].arcs[path[k]].id_to].importance++;
			}
			//if(j%100==0)
			printf("已完成第%d个\n",i*1000+j+1);
		}
	}

	if(size%1000!=0)
	{
		//itoa(i,index,10);
		sprintf(index,"%d",i);
		strcpy(filename,inputpath);
		strcat(filename,"sptree");
		strcat(filename,index);
		FILE *fp_tree=fopen(filename,"rb");
		for(int j=0;j<size%1000;j++)
		{
			//Dijkstra(i*1000+j,path,label,dist);
			fread(path,sizeof(unsigned char)*size,1,fp_tree);
			for(int k=1;k<size;k++)
			{
				nodes[k].arcs[path[k]].importance++;
				nodes[nodes[k].arcs[path[k]].id_to].importance++;
			}
			//if(j%100==0)
			printf("已完成第%d个\n",i*1000+j+1);
		}
	}
	FILE *fp=fopen("E:\\GraphData\\importance.txt","w");
	for(int i=0;i<size;i++)
	{
		if(i%1000==0)
		printf("已完成%d\n",i);
		fprintf(fp,"%d",nodes[i].importance);
		for(int j=0;j<nodes[i].degree;j++)
			fprintf(fp," %d",nodes[i].arcs[j].importance);
		fprintf(fp,"\n");
	}
	fclose(fp);


}

void Graph::Compress(char *dirpath,int a,int b)
{
	int sum=0;//已添加的记录总数
	int record_size;//单条记录压缩后大小
	unsigned char *path=new unsigned char[size];
	int *dist=new int[size];
	int *label=new int[size];
	unsigned char *tmp_dic;//暂时存放字典
	//unsigned char *dest=new unsigned char[size];
	unsigned char *buf[2];
	buf[0]=new unsigned char[size];
	buf[1]=new unsigned char[size];
	vector<int> order;
			
	double sum_unpack=0;
	/*int a=0,b=0;
	scanf("%d%d",&a,&b);*/
	if(b>n_regions)
		b=n_regions;
	
	for(int i=a;i<b;i++)
	{
		char filename[300]={0};
		char b[20]={0};
		sprintf(b,"%d",i);
		//itoa(i,b,10);
		strcpy(filename,dirpath);
		strcat(filename,b);
		//FILE *fp=fopen(filename,"rb");

		if(access(filename, 0) == 0)
		{
			//fclose(fp);
			printf("Process %d 跳过%d\n",omp_get_num_threads(),i);fflush(stdout);
			continue;
		}
		order.clear();
		tmp_dic=buf[0];
		
		//求根字典
		Dijkstra(regions[i].nodes[0],tmp_dic,label,dist);
		record_size=regions[i].data->AddRecord(regions[i].data->dictionary,tmp_dic);
		size_current+=record_size;
		//求依赖关系树
		int *tree=regions[i].data->depend_tree;//new int[regions[i].size];
		//regions[i].data->depend_tree=tree;
		/*tree[0]=0;

		for(int j=1;j<regions[i].size;j++)
		{
			tree[j]=regions[i].nodes[j];
			int region=i;
			for(int k=0;k<STEP&&tree[j]!=regions[i].nodes[0];k++)//
				tree[j]=nodes[tree[j]].arcs[tmp_dic[tree[j]]].id_to;
			int cc=0;
			while(nodes[tree[j]].id_region!=region)
			{
				cc++;
				//printf("区域%d结点%d出界%d啦啦啦啦！！！！\n",i,j,cc);
				tree[j]=nodes[tree[j]].arcs[tmp_dic[tree[j]]].id_to;
			}
			if(cc!=0)
				printf("区域%d结点%d出界%d啦啦啦啦！！！！\n",i,j,cc);
			tree[j]=nodes[tree[j]].id_in_region;
		}
		*/
		int n_steps=0;
		int dic_id=0;//当前作为字典的结点编号
		tmp_dic=regions[i].data->dictionary;//当前字典串
		//对区域内每条记录进行压缩
		for(int j=1;j<regions[i].size;j++)
		{
			sum++;
			if(dic_id!=tree[j])//当前需要压缩的数据不依赖当前字典
			{
				dic_id=tree[j];
				//printf("Error: try to get a non-existent record %d, max is %d\n",tree[j],j);
	
				tmp_dic=regions[i].data->GetRecord(buf,tree[j],false,n_steps);//获得待压缩数据的依赖数据
			}
			//QueryPerformanceCounter( &begin );
			Dijkstra(regions[i].nodes[j],path,label,dist);
			//QueryPerformanceCounter( &end );
			//totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
			//printf("Dijkstra时间=%fus\n",totaltime);


			//QueryPerformanceCounter( &begin );
			record_size=regions[i].data->AddRecord(tmp_dic,path);
			//size_current+=record_size;
			//QueryPerformanceCounter( &end );
			//totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
			//printf("编码时间=%fus\n",totaltime);

			//QueryPerformanceCounter( &begin );
			unsigned char *result=regions[i].data->GetRecord(buf,j,false,n_steps);
			//QueryPerformanceCounter( &end );
			//totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
			//sum_unpack+=totaltime;
			//printf("解码时间=%fus\n",totaltime);

			if(memcmp(result,path,size)!=0)
			{
				printf("区域%d结点%d解码出错！！！！！！\n",i,j);
				FILE *err=fopen("err.txt","w");
				for(int xx=1;xx<size;xx++)
					if(path[xx]!=result[xx])
						fprintf(err,"%d %d %d\n",xx,path[xx],result[xx]);
				fclose(err);
				system("pause");
				/**/
				//return;
			}
			if(j%10==0)
			{
				printf("region %d/%d node %d/%d finished.\n",i,n_regions-1,j,regions[i].size-1);

				int size_raw=regions[i].data->size_raw;
				int size_current=regions[i].data->size_current;
				printf("区域压缩比:%.2f,单条记录压缩比：%.2f\n",1.0*(size_raw)/(size_current+size),1.0*size/(record_size+1));
			}
			fflush(stdout);
		}
		regions[i].data->Save(dirpath);
		delete tree;
		delete regions[i].data->dictionary;
		delete regions[i].data;
		//printf("%d\n",i);
	}
	delete path;
	delete label;
	delete dist;
	delete buf[0];
	delete buf[1];
	
	//printf("一共消耗时间：%f\n",sum_unpack);
	//printf("平均解压时间：%f\n",sum_unpack/sum);
}

void Graph::Decompress_prepare(char *dirpath,bool exMemory)
{
	for(int i=0;i<n_regions;i++)
	{
		regions[i].data->Load(dirpath,exMemory);
		//printf("%d\n",i);
	}
}

double Graph::evaluate()//评估区域划分效果
{
	double sum=0;
	unsigned char *path=new unsigned char[size];
	int *dist=new int[size];
	int *label=new int[size];
	
	for(int i=0;i<n_regions;i++)
	{
		Dijkstra(regions[i].nodes[0],path,label,dist);
		for(int j=1;j<regions[i].size;j++)
			sum+=dist[regions[i].nodes[j]]; //CalculateDistance(nodes[regions[i].nodes[0]].x,nodes[regions[i].nodes[j]].x,nodes[regions[i].nodes[0]].y,nodes[regions[i].nodes[j]].y);
		printf("正在计算区域%d的距离和\n",i);
	}
	//printf("结点到中心的距离和为：%f\n",log(sum));
	return log(sum);
}

void Graph::BFM(int source,unsigned char *path,int *label)
{
	bool *mark=new bool[size];
	int *tmp_path=new int[size];
	queue<int> Q;
	memset(mark,0,sizeof(bool)*size);
	//memset(label,0,size*sizeof(int));
	for(int i=0;i<size;i++)
		label[i]=0x7fffffff;
	//memset(tmp_path,0,size*sizeof(int));
	
	label[source]=0;
	Q.push(source);
	mark[source]=true;
	while(!Q.empty())
	{
		int u=Q.front();
		Q.pop();
		mark[u]=false;
		for(int i=0;i<nodes[u].degree;i++)
		{
			int id_to=nodes[u].arcs[i].id_to;
			if(label[u]+nodes[u].arcs[i].weight<label[id_to])
			{
				tmp_path[id_to]=u;
				if(!mark[id_to])
				{
					Q.push(id_to);
					mark[id_to]=true;
				}
			}
		}
	}
	for(int i=1;i<size;i++)
		for(int j=0;j<nodes[i].degree;j++)
			if(nodes[i].arcs[j].id_to=tmp_path[i])
			{
				path[i]=j;
				break;
			}
	path[0]=0;
	delete mark;
	delete tmp_path;
}

int Graph::Astar(int source,int target,int *path,int *label,double epsilon)
{
	int id=source;
	int count_searched=0;
	int *hash=new int[size];
	int *weight=new int[size];//weight指示source到当前结点的最短距离
	bool *mark=(bool*)calloc(size,sizeof(bool));
	for (int i = 0; i < size; i++)
    {
        hash[i] = -1;
		//label[i]=INFINITY;
    }
	memset(label,0,size*sizeof(int));
	PriorityQueue Q(1000,label,hash);

	path[0]=0;
	path[source]=source;
    label[source] = 0;
    while(id!=target)
    {   
		for (int i=0;i<nodes[id].degree;i++)
		{
			int id_to=nodes[id].arcs[i].id_to;
			if (!mark[id_to])// weight[id] + nodes[id].arcs[i].weight < label[nodes[id].arcs[i].id_to])
            {
				double estimation=CalculateDistance(nodes[id_to].x,nodes[target].x,nodes[id_to].y,nodes[target].y);
				if(label[id_to] == 0 || weight[id] + nodes[id].arcs[i].weight + epsilon * estimation < label[id_to])
				{
					weight[id_to]=weight[id] + nodes[id].arcs[i].weight;
					label[id_to] = weight[nodes[id].arcs[i].id_to] + epsilon * estimation;
					if (hash[id_to] == -1)
					{
						Q.Push(id_to);
					}
					else
					{
						Q.Adjust(hash[id_to]);
					}
					path[id_to] = id;
				}
            }
		}
		count_searched++;
		mark[id]=true;
        id = Q.Top();
        Q.Pop();
    }
	path[source]=weight[target];
	delete hash;
	delete weight;
	delete mark;
	return count_searched;
}
