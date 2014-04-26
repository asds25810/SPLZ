#define INFINITY 2147483647
//#define INFINITY_DOUBLE  
#include "..\include\PriorityQueue.h"
#include "..\include\head.h"
#include <io.h>
#include <hash_map>

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
int Graph::LoadGraph(char *filepath)//载入图信息
{
	int *maxdegree;
	double sum_arc=0;
	FILE *fp=fopen(filepath,"r");
	if(!fp)
	{
		printf("Error: cannot open the file: %s\n",filepath);
		return 0;
	}
	char buf[200]={0};
	//skip some information
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
	printf("finish loading graph data\n");
	if(max>15)
		printf("Warning: the max degree of vertex in this graph is more than 15. SPLZ cannot calculate a correct result.\n");

	delete maxdegree;
	size_current=0;
	count_current=0;
	return 1;
}

int Graph::LoadCoordinate(char *filename)//载入坐标信息
{
	FILE *fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error: cannot open the file: %s\n",filename);
		return 0;
	}
	char buf[200]={0};
	//skip some information
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
	return 1;
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
#define DIST(x1,x2,y1,y2) ((x1*1.0-x2)*(x1-x2)+(y1*1.0-y2)*(y1-y2))
void Graph::Segment(int k,int n, char* filename)
{
	n_regions=k;
	__int64 *sum_x=(__int64*)calloc(k,sizeof(__int64));//用于计算区域中心
	__int64 *sum_y=(__int64*)calloc(k,sizeof(__int64));//用于计算区域中心
	double sum=0;
	double last=0;
	int *count=new int[k];//区域大小计数
	int *size_max=new int[k];
	for(int i=0;i<k;i++)
		size_max[i]=size/k;
	for(int i=0;i<size%k;i++)
		size_max[i]++;

	regions=(region*)calloc(k,sizeof(region));
	//random initial
	int *center_x=new int[k];
	int *center_y=new int[k];
	for(int i=0;i<k;i++)
	{
		center_x[i]=x_west+rand()%(x_east-x_west);
		center_y[i]=y_south+rand()%(y_north-y_south);
	}
	bool flag=false;
	for(int j=0;j<n;j++)//begin to iterate
	{
		//calculate which center a vertex belongs to
		memset(count,0,sizeof(int)*k);
		sum=0;
		flag=false;
		#pragma omp parallel for reduction(+: sum)
		for(int i=1;i<size;i++)
		{
			double min_distance=DBL_MAX;
			int center_index=0;
			double dist=0;
			for(int l=0;l<k;l++)
			{
				if(count[l]>=size_max[l]&&j<n/2)
					continue;
				//dist=CalculateDistance(nodes[i].x,center_x[l],nodes[i].y,center_y[l]);
				dist=DIST(nodes[i].x,center_x[l],nodes[i].y,center_y[l]);
				if(dist<min_distance)
				{
					min_distance=dist;
					center_index=l;
				}
			}
			sum+=sqrt(min_distance);
			//_fpclass(sum)
			nodes[i].id_region=center_index;
			#pragma omp atomic
			count[center_index]++;
		}

		//calculate the new center of each region
		double max_move=0;
		double t;
		memset(sum_x,0,sizeof(__int64)*k);
		memset(sum_y,0,sizeof(__int64)*k);
		for(int i=1;i<size;i++)
		{
			sum_x[nodes[i].id_region]+=nodes[i].x;
			sum_y[nodes[i].id_region]+=nodes[i].y;
		}
		for(int l=0;l<k;l++)
		{
			if(count[l]==0)//其实，出现这种情况的最好办法是删掉一个region
			{
				int seed=rand()%(size-1)+1;
				center_x[l]=nodes[seed].x;//x_west+rand()%(x_east-x_west);
				center_y[l]=nodes[seed].y;//y_south+rand()%(y_north-y_south);
				if(!flag)
				{
					n++;
					flag=true;
				}
				continue;
			}
			center_x[l]=sum_x[l]/count[l];
			center_y[l]=sum_y[l]/count[l];
		}
		
		printf("Partition iteration %d finish. log(sum of dist) is %f\n",j+1,log(sum+1));
		if(fabs(log(sum+1)-last)<0.01)
			break;
		last=log(sum+1);
	}

	//count the number of vertices in each region
	for(int i=0;i<k;i++)
	{
		regions[i].nodes.resize(count[i]);//=new int[count[i]];
	}
	for(int i=1;i<size;i++)
	{
		nodes[i].id_in_region=regions[nodes[i].id_region].size;
		regions[nodes[i].id_region].nodes[regions[nodes[i].id_region].size]=i;
		regions[nodes[i].id_region].size++;
	}

	//select a representative vertex for every region
	for(int i=0;i<k;i++)
	{
		/*if(regions[i].size==0)
			printf("regions %d size==0\n",i);
		if(regions[i].size<10)
			printf("regions %d size==%d\n",i,regions[i].size);*/
		int present;
		double min_dist=DBL_MAX;
		double dist;
		for(int j=0;j<regions[i].size;j++)
		{
			if((dist=CalculateDistance(nodes[regions[i].nodes[j]].x,center_x[i],nodes[regions[i].nodes[j]].y,center_y[i]))<min_dist)
			{
				min_dist=dist;
				present=j;
			}
		}
		//set the representative vertex as the first vertex
		int t;
		t=regions[i].nodes[0];
		regions[i].nodes[0]=regions[i].nodes[present];
		regions[i].nodes[present]=t;
		nodes[regions[i].nodes[0]].id_in_region=0;
		nodes[regions[i].nodes[present]].id_in_region=present;

	}

	Reorder(filename);

	printf("finish partitioning.\n");

	delete count;
}
void Graph::Reorder(char *filename)
{
	FILE *tmp_dist=fopen("tmp_dist","wb");
	FILE *tmp_path=fopen("tmp_path","wb");

	int buf_size=200;
	unsigned char *dest=new unsigned char[size];
	vector<int> order;
	//partition the graph by path-len
	int *center=new int[n_regions];
	for(int i=0;i<n_regions;i++)
		center[i]=regions[i].nodes[0];
	int **dist=new int*[buf_size];
	unsigned char **path=new unsigned char*[buf_size];
	for(int i=0;i<buf_size;i++)
	{
		dist[i]=new int[size];
		path[i]=new unsigned char[size];
	}
	int k=0;

	for(k=0;k<n_regions/buf_size;k++)
	{
		#pragma omp parallel for
		for(int i=0;i<buf_size;i++)
		{
			int *label=new int[size];
			Dijkstra(regions[k*buf_size+i].nodes[0],path[i],label,dist[i]);
			printf("重新划分区域%d\n",k*buf_size+i);
			delete label;
		}
		for(int i=0;i<buf_size;i++)
		{
			fwrite(dist[i],size,sizeof(int),tmp_dist);
			fwrite(path[i],size,sizeof(unsigned char),tmp_path);
		}
	}
	#pragma omp parallel for
	for(int i=0;i<n_regions%buf_size;i++)
	{
		int *label=new int[size];
		Dijkstra(regions[k*buf_size+i].nodes[0],path[i],label,dist[i]);
		printf("重新划分区域%d\n",k*buf_size+i);
		delete label;
	}
	for(int i=0;i<n_regions%buf_size;i++)
	{
		fwrite(dist[i],size,sizeof(int),tmp_dist);
		fwrite(path[i],size,sizeof(unsigned char),tmp_path);
	}
	fclose(tmp_dist);
	fclose(tmp_path);

	tmp_dist=fopen("tmp_dist","rb");
	double *min_distance=new double[size];
	for(int i=0;i<size;i++)
		min_distance[i]=DBL_MAX;
	//计算每个结点归属的中心
	int *count=new int[n_regions];
	memset(count,0,sizeof(int)*n_regions);
	for(k=0;k<n_regions/buf_size;k++)
	{
		for(int i=0;i<buf_size;i++)
			fread(dist[i],size,sizeof(int),tmp_dist);
		#pragma omp parallel for
		for(int i=1;i<size;i++)
		{
			int center_index=nodes[i].id_region;
			int d=0;
			for(int l=0;l<buf_size;l++)
			{
				d=dist[l][i];
				if(d<min_distance[i])
				{
					min_distance[i]=d;
					center_index=k*buf_size+l;
				}
			}
			nodes[i].id_region=center_index;
		}
	}
	for(int i=0;i<n_regions%buf_size;i++)
		fread(dist[i],size,sizeof(int),tmp_dist);
	#pragma omp parallel for
	for(int i=1;i<size;i++)
	{
		int center_index=nodes[i].id_region;
		int d=0;
		for(int l=0;l<n_regions%buf_size;l++)
		{
			d=dist[l][i];
			if(d<min_distance[i])
			{
				min_distance[i]=d;
				center_index=k*buf_size+l;
			}
		}
		nodes[i].id_region=center_index;
	}
	fclose(tmp_dist);
	for(int i=1;i<size;i++)
		count[nodes[i].id_region]++;

	//统计区域中包含的结点
	memset(regions,0,sizeof(region)*n_regions);
	for(int i=0;i<n_regions;i++)
	{
		regions[i].nodes.resize(count[i]);//=new int[count[i]];
	}
	for(int i=1;i<size;i++)
	{
		nodes[i].id_in_region=regions[nodes[i].id_region].size;
		regions[nodes[i].id_region].nodes[regions[nodes[i].id_region].size]=i;
		regions[nodes[i].id_region].size++;
	}
	//将代表元调整为区域中第一个元素
	for(int i=0;i<n_regions;i++)
	{
		for(int k=0;k<regions[i].size;k++)
			if(regions[i].nodes[k]==center[i])
			{
				printf("error!!!!!!!!!\n");
				break;
			}
		int t;
		t=regions[i].nodes[0];
		regions[i].nodes[0]=regions[i].nodes[nodes[center[i]].id_in_region];
		regions[i].nodes[nodes[center[i]].id_in_region]=t;
		nodes[regions[i].nodes[0]].id_in_region=0;
		nodes[regions[i].nodes[nodes[center[i]].id_in_region]].id_in_region=nodes[center[i]].id_in_region;
	}

	FILE *fp=fopen(filename,"w");
	tmp_path=fopen("tmp_path","rb");
	fprintf(fp,"%d\n",n_regions);

	FILE *fp_order=fopen("order.txt","w");

	for(int i=0;i<n_regions;i++)
	{
		printf("重新编号%d\n",i);
		fread(path[0],size,sizeof(unsigned char),tmp_path);
		order.clear();
		//求根字典
		//Dijkstra(regions[i].nodes[0],path,label,dist);
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
				tree[j]=nodes[tree[j]].arcs[path[0][tree[j]]].id_to;
			}
			int c=0;
			while(nodes[tree[j]].id_region!=region)
			{
				c++;
				tree[j]=nodes[tree[j]].arcs[path[0][tree[j]]].id_to;
			}
			if(c!=0)
			{
				printf("出界！！！！超出预定距离%d\n",++c);
			}
			tree[j]=nodes[tree[j]].id_in_region;
		}
		//区域内宽度优先重排
		//宽度优先序
		Get_Order_BFS(&order,tree,regions[i].size);
		
		vector<int> mapping;
		mapping.resize(regions[i].size);
		fprintf(fp_order,"%d",order.size());
		for(int k=0;k<regions[i].size;k++)
			mapping[order[k]]=k;
		for(int k=0;k<regions[i].size;k++)
			fprintf(fp_order," %d",mapping[tree[order[k]]]);
		fprintf(fp_order,"\n");
		for(int k=1;k<regions[i].size;k++)
		{
			if(mapping[tree[order[k]]]<mapping[tree[order[k-1]]])
				printf("出错！！！！！！序列非增\n");
		}
		//排好之后输出到文件
		fprintf(fp,"%d",regions[i].size);
		for(int l=0;l<regions[i].size;l++)
		{
			fprintf(fp," %d",regions[i].nodes[order[l]]);
		}
		fprintf(fp,"\n");
		for(int l=0;l<regions[i].size;l++)
		{
			fprintf(fp," %d",mapping[tree[order[l]]]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp_order);
	fclose(fp);
	fclose(tmp_path);
	remove("tmp_dist");
	remove("tmp_path");
}
		
int Graph::LoadRegion(char *filename)
{
	FILE *fp=fopen(filename,"r");
	if(!fp)
	{
		printf("cannot open file: %s\n",filename);
		return 0;
	}
	fscanf(fp,"%d",&n_regions);
	regions=new region[n_regions];
	memset(regions,0,sizeof(region)*n_regions);
	for(int i=0;i<n_regions;i++)
	{
		fscanf(fp,"%d",&regions[i].size);
		regions[i].nodes.resize(regions[i].size);
		for(int j=0;j<regions[i].size;j++)
		{
			fscanf(fp,"%d",&regions[i].nodes[j]);
			nodes[regions[i].nodes[j]].id_region=i;
			nodes[regions[i].nodes[j]].id_in_region=j;
		}
		
		//allocate the memory for compressed data
		regions[i].data=new DataBlock;
		regions[i].data->id=i;
		//regions[i].data->nodes=nodes;
		regions[i].data->init(size,regions[i].size,size/2000*2+100);
		regions[i].data->depend_tree=new int[regions[i].size];
		//load the dependent tree
		for(int j=0;j<regions[i].size;j++)
		{
			fscanf(fp,"%d",&regions[i].data->depend_tree[j]);
		}
	}
	fclose(fp);
	return 1;
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

	if(b>n_regions)
		b=n_regions;
	
	for(int i=a;i<b;i++)
	{
		char filename[300]={0};
		char b[20]={0};
		sprintf(b,"%d",i);
		strcpy(filename,dirpath);
		strcat(filename,b);

		if(access(filename, 0) == 0)
		{
			printf("Thread %d skips region %d\n",omp_get_num_threads(),i);fflush(stdout);
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

			Dijkstra(regions[i].nodes[j],path,label,dist);

			record_size=regions[i].data->AddRecord(tmp_dic,path);

			unsigned char *result=regions[i].data->GetRecord(buf,j,false,n_steps);

			if(memcmp(result,path,size)!=0)
			{
				printf("区域%d结点%d解码出错！！！！！！\n",i,j);
				/*FILE *err=fopen("err.txt","w");
				for(int xx=1;xx<size;xx++)
					if(path[xx]!=result[xx])
						fprintf(err,"%d %d %d\n",xx,path[xx],result[xx]);
				fclose(err);*/
				system("pause");
			}
		}
		double compression_ratio=1.0*(regions[i].data->size_raw)/(regions[i].data->size_current+size);
		printf("region %d has been compressed. compression ratio is %.2f\n",i,compression_ratio);
		regions[i].data->Save(dirpath);
		delete tree;
		delete regions[i].data->index;
		delete regions[i].data->records;
		delete regions[i].data->dictionary;
		delete regions[i].data;
	}
	delete path;
	delete label;
	delete dist;
	delete buf[0];
	delete buf[1];

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


