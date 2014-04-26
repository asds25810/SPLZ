#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "KMP.h"
#include <omp.h>


#define NUMBER_OF_SEGMENTS 256
#define NUMBER_OF_THREADS 256
#define DECODE_CHECK 1
typedef int record_for_mt[NUMBER_OF_SEGMENTS+1];

class DataBlock
{
public:
	int id;
	int size_max;//压缩数据最大大小    
	int size_raw;//原始数据大小    //此处有溢出风险。。。。。。。！！！！！！
	int count;//总记录数

	int length_segment;//每一段的长度 用于多线程
	int length;//每条记录的长度  size_raw==length*count
	int count_current;//当前记录条数
	int size_current;//当前压缩数据大小  按sizeof(unsigned char)计
	int *depend_tree;
	FILE *data_in_file;
	//HANDLE data_in_file;//由于fopen最大打开文件数的限制,改用CreateFile系列windows API代替.
	//vertex *nodes;////这是干啥的？？？？//似乎是为了求依赖树，不过用depend_tree代替了，应该没用了。
	unsigned char *data;//压缩数据
	int *index;//长度为count，index[i]为第i条记录在压缩数据中的起始地址
	record_for_mt *records;//为多线程建立的索引
	unsigned char *dictionary;//字典长度即length
	//Trie *dictionary;

	//DataBlock(int length,int count,int init_size,int dic_size,int dic_degree);
	void init(int length,int count,int init_size);//int dic_size,int dic_degree);
	int AddRecord(unsigned char *dictionary,unsigned char *source);
	unsigned char* GetRecord(unsigned char* buf[2],int index,bool exMemory,int &n_steps);
	void Encode(unsigned char *dictionary,unsigned char *source);
	void Decode(unsigned char *dictionary,unsigned char *dest,unsigned char *source,int length_source);
	void Decode_MT(unsigned char *dictionary,unsigned char *dest,unsigned char *source,int length_source,int length_dest,record_for_mt record);
	void Save(char *dirpath);
	void Load(char *dirpath,bool exMemory);
};

void DataBlock::init(int length,int count,int init_size)//int dic_size,int dic_degree)
{
	this->length=length;
	this->count=count;
	//this->size_raw=length*count;
	this->size_max=init_size;
	this->count_current=0;
	this->size_current=0;
	this->size_raw=length*count_current;
	this->records=new record_for_mt[count];
	this->length_segment=ceil((double)(length/NUMBER_OF_SEGMENTS));
	dictionary =new unsigned char[length];//Trie(dic_size,dic_degree);
	index=new int[count+1];
	//records=new record_for_mt[count_max];
	data=new unsigned char[init_size];
}

int DataBlock::AddRecord(unsigned char *dictionary,unsigned char *source)
{
	
	if(count_current==0)//第一条数据(区域代表元)作为字典
	{
		memcpy(dictionary,source,length);
		index[count_current]=size_current;
		//size_current+=0;
		count_current++;
		return 0;
	}

	index[count_current]=size_current;
	
	int size_now=size_current;
	this->size_raw=length*count_current;
	Encode(dictionary,source);
	count_current++;
	index[count_current]=size_current;
	
	if(count_current==count)//记录数已满
	{
		data=(unsigned char*)realloc(data,size_current*sizeof(unsigned char));
		index[count_current]=size_current;
		//return -1;
	}
	return (size_current-size_now);
}

unsigned char* DataBlock::GetRecord(unsigned char *buf[2],int index,bool exMemory,int &n_steps)
{
	if(index==0)
	{
		memcpy(buf[0],dictionary,length);
		return buf[0];
	}
	if(index>this->count_current)
	{
		//printf("Error: try to get a non-existent record %d, max is %d\n",index,this->count_current);
	}


	//先求出依赖路径
	int begin_in_file=sizeof(DataBlock)+length+(count+count+1)*sizeof(int)+count*sizeof(record_for_mt);//DataBlock + dictionary + index + depend_tree +record
	int parent=index;
	vector<int> path;
	while(parent!=0)
	{
		path.push_back(parent);
		parent=depend_tree[parent];
	}
	index=path.back();
	path.pop_back();
	int length_source=this->index[index+1]-this->index[index];//对应的压缩数据长度
	if(exMemory)
	{
		unsigned long read_size=0;

		fseek(data_in_file,begin_in_file+this->index[index],0);
		fread(data,length_source,read_size,data_in_file);
		
		Decode_MT(this->dictionary,buf[0],data,length_source,length,records[index]);
		//Decode(this->dictionary,buf[0],data,length_source);
	}
	else
	{
		Decode_MT(this->dictionary,buf[0],&data[this->index[index]],length_source,length,records[index]);
		//Decode(this->dictionary,buf[0],&data[this->index[index]],length_source);
	}
	//printf("解压经过了%d步\n",path.size()+1);
	int c=0;
	n_steps=1;
	while(!path.empty())
	{
		index=path.back();
		path.pop_back();
		c=1-c;
		n_steps++;
		int length_source=this->index[index+1]-this->index[index];//对应的压缩数据长度
		if(exMemory)
		{
			unsigned long read_size=0;

			fseek(data_in_file,begin_in_file+this->index[index],0);
			fread(data,length_source,read_size,data_in_file);

			Decode_MT(buf[1-c],buf[c],data,length_source,length,records[index]);
		}
		else
			Decode_MT(buf[1-c],buf[c],&data[this->index[index]],length_source,length,records[index]);
	}

	return buf[c];
}
#define WINDOW_SIZE 2000
void DataBlock::Encode(unsigned char *dictionary,unsigned char *source)
{
	records[count_current][0]=size_current;
	int segment_finished=1;
	int length_segment=ceil((double)(length/NUMBER_OF_SEGMENTS))+1;

 	int *fail=new int[length];
	KMP_CalculateFail(source,fail,length);//求KMP失配数组
	int source_index=0;//source数组的当前位置
	int last_location=0;
	while(source_index!=length)
	{
		int max_location=0,max_length=0;//最长匹配的位置和匹配长度
		int dictionary_index=(source_index-WINDOW_SIZE<=0?0:source_index-WINDOW_SIZE);//dictionary数组的当前位置
		int i=source_index;//i表示source中当前已经匹配到的位置
		while(dictionary_index<length //字典未到尾
			   && i<length)           //源数据未处理完
		{
			//开始匹配直到失配或者到达字典末尾或者到达源数据末尾，记录最长匹配的位置和匹配长度。
			while(dictionary_index<length && 
				  i<length && source[i]==dictionary[dictionary_index] &&
				  i%length_segment!=length_segment-1)//到达分段点
			{
				dictionary_index++;
				i++;
			}
			if(i-source_index>max_length)
			{
				max_location=dictionary_index-(i-source_index);
				max_length=i-source_index;
			}
			if(dictionary_index>=length || i>=length || (i)%length_segment==length_segment-1)//到达字典末尾或者到达源数据末尾或者到达分段点
				break;
			if(max_length!=0&&dictionary_index>=(source_index+WINDOW_SIZE>length?length:source_index+WINDOW_SIZE))
				break;
			i=fail[i];//失配滑动
			if(source_index>i)
			{
				/*
					dictionary_index+=i-source_index;
					i=source_index;
				*/
				i=source_index;
				dictionary_index++;
			}				
		}

		//输出压缩数据
		unsigned char buf[4]={0};
		if(size_current+10>=size_max)
		{
			data=(unsigned char*)realloc(data,size_current*2*sizeof(unsigned char));
			size_max=size_current*2;
		}
		if(max_length==0)// 1111 0xxx    修改为 1111 xxxx
		{
			//printf("未能进行压缩！！！%d\n",(int)source[source_index]);
			data[size_current]=(source[source_index]|0xf0);//&0xf7; 修改
			size_current++;
			source_index++;	
			continue;
		}
		else if(max_length<=0x7f)// 0xxx xxxx
		{
			data[size_current]=max_length&0x7f;
			size_current++;
		}
		else if(max_length<=0x3fff)// 10xx xxxx xxxx xxxx
		{
			data[size_current]=((max_length&0xff00)>>8)|0x80;
			data[size_current+1]=max_length&0xff;
			size_current+=2;
		}
		else if(max_length<=0x1fffff)// 110x xxxx xxxx xxxx xxxx xxxx
		{
			data[size_current]=((max_length&0xff0000)>>16)|0xc0;
			data[size_current+1]=(max_length&0xff00)>>8;
			data[size_current+2]=max_length&0xff;
			size_current+=3;
		}
		else if(max_length<=0x0fffffff)// 1110 xxxx xxxx xxxx xxxx xxxx xxxx xxxx
		{
			data[size_current]=((max_length&0xff000000)>>24)|0xe0;
			data[size_current+1]=(max_length&0xff0000)>>16;
			data[size_current+2]=(max_length&0xff00)>>8;
			data[size_current+3]=max_length&0xff;
			size_current+=4;
		}
		else
			printf("length超出长度范围！！！\n");

		max_location=max_location-last_location;//不是记录实际值，而是记录差值
		//fprintf(fp,"%d %d %d\n",max_length,last_location,max_location);
		last_location=last_location+max_location;
		
		bool flag=(max_location<0);
		max_location=abs(max_location);
		if(max_location<=0x3f)//&&max_location>=-0x3f)// 0xxx xxxx
		{
			data[size_current]=max_location&0x7f;
			if(flag)
				data[size_current]=data[size_current]|0x40;//01xx xxxx
			size_current++;
		}
		else if(max_location<=0x1fff)//&&max_location>=-0x1fff)// 10xx xxxx xxxx xxxx
		{
			data[size_current]=((max_location&0xff00)>>8)|0x80;
			data[size_current+1]=max_location&0xff;
			if(flag)
				data[size_current]=data[size_current]|0x20;//101x xxxx
			size_current+=2;
		}
		else if(max_location<=0x0fffff)//&&max_location>=-0x0fffff)// 110x xxxx xxxx xxxx xxxx xxxx
		{
			data[size_current]=((max_location&0xff0000)>>16)|0xc0;
			data[size_current+1]=(max_location&0xff00)>>8;
			data[size_current+2]=max_location&0xff;
			if(flag)
				data[size_current]=data[size_current]|0x10;//1101 xxxx
			size_current+=3;
		}
		else if(max_location<=0x07ffffff)//&&max_location>=-0x07ffffff)// 1110 xxxx xxxx xxxx xxxx xxxx xxxx xxxx
		{
			data[size_current]=((max_location&0xff000000)>>24)|0xe0;
			data[size_current+1]=(max_location&0xff0000)>>16;
			data[size_current+2]=(max_location&0xff00)>>8;
			data[size_current+3]=max_location&0xff;
			if(flag)
				data[size_current]=data[size_current]|0x08;//1110 1xxx
			size_current+=4;
		}
		else
			printf("location超出长度范围！！！\n");
		source_index+=max_length;
		if(source_index%length_segment==length_segment-1 ||source_index==length)
		{
			records[count_current][segment_finished]=size_current;//第segment_finished段的起始位置
			segment_finished++;
			last_location=0;
		}
	}
	records[count_current][NUMBER_OF_SEGMENTS]=size_current;//末尾
	for(int i=NUMBER_OF_SEGMENTS;i>=0;i--)
		records[count_current][i]-=records[count_current][0];
	//fclose(fp);
	delete fail;
}

inline void DataBlock::Decode(unsigned char *dictionary,unsigned char *dest,unsigned char *source,int length_source)
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

void DataBlock::Decode_MT(unsigned char *dictionary,unsigned char *dest,unsigned char *source,int length_source,int length_dest,record_for_mt record)
{
	int length_segment=ceil((double)(length_dest/NUMBER_OF_SEGMENTS))+1;
	//#pragma omp parallel for
	for(int i=0;i<NUMBER_OF_THREADS;i++)
	{
		int begin=-1+length_segment*i<0?0:-1+length_segment*i;
		int l=length_segment;
		if(i==0)
			l=length_segment-1;
		if(i==NUMBER_OF_THREADS-1)
			l=length_dest-length_segment*(NUMBER_OF_THREADS-1)+1;
		Decode(dictionary,&dest[begin],&source[record[i]],record[i+1]-record[i]);
	}
}

void DataBlock::Save(char *dirpath)
{
	char filename[300]={0};
	char buf[20]={0};
	sprintf(buf,"%d",id);
	//itoa(id,buf,10);
	strcpy(filename,dirpath);
	strcat(filename,buf);
	FILE *fp=fopen(filename,"wb");
	fwrite(this,sizeof(DataBlock),1,fp);
	fwrite(this->dictionary,length*sizeof(unsigned char),1,fp);
	fwrite(this->index,(count+1)*sizeof(int),1,fp);
	fwrite(this->records,count*sizeof(record_for_mt),1,fp);
	fwrite(this->depend_tree,count*sizeof(int),1,fp);
	fwrite(data,sizeof(unsigned char)*size_current,1,fp);
	fclose(fp);
}

void DataBlock::Load(char *dirpath,bool exMemory)
{
	char filename[300]={0};
	char buf[20]={0};
	sprintf(buf,"%d",id);
	//itoa(id,buf,10);
	strcpy(filename,dirpath);
	strcat(filename,buf);
	FILE *fp=fopen(filename,"rb");
	fread(this,sizeof(DataBlock),1,fp);
	index=new int[count+1];
	records=new record_for_mt[count];
	depend_tree=new int[count];
	dictionary=new unsigned char[length];
	fread(this->dictionary,length*sizeof(unsigned char),1,fp);
	fread(this->index,(count+1)*sizeof(int),1,fp);
	fread(this->records,count*sizeof(record_for_mt),1,fp);
	fread(this->depend_tree,count*sizeof(int),1,fp);
	if(!exMemory)
	{
		data=new unsigned char[size_current];
		fread(data,sizeof(unsigned char)*size_current,1,fp);
		fclose(fp);
	}
	else
	{
		data=new unsigned char[length];//此处默认了压缩数据一定比原始数据大。在理论上其实并不一定满足。。
		fclose(fp);
		data_in_file=fopen(filename,"rb");
	}
	
}

