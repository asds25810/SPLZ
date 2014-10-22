#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <io.h>
#include <math.h>
#include <omp.h>
#include "..\include\Graph.h"

#define N 100000


int GetDist(int v, int *parent, int *dist, Graph &graph)
{
	//if(parent[v]!=0)
	//{
	//	if(dist[parent[v]==0)
	//		return GetDist
	//}
}

int main()
{
	LARGE_INTEGER begin,end,lv;
	double totaltime,secondsPerTick;
	QueryPerformanceFrequency(&lv);
	secondsPerTick=1000000.0/lv.QuadPart;
	srand(time(0));

	Graph graph("E:\\USA-road-d.NW.gr");
	graph.LoadCoordinate("E:\\USA-road-d.NW.co");
	graph.LoadRegion("E:\\regions_Kmeans_1.txt");

	QueryPerformanceCounter( &begin );
	graph.Decompress_prepare("E:\\regions_Kmeans_1_64\\",false);
	QueryPerformanceCounter( &end );
	totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
	printf("prepare time=%fus\n",totaltime);

	int s=1;
	int n_steps=0;
	unsigned char *buf[2];
	buf[0]=new unsigned char[graph.size];
	buf[1]=new unsigned char[graph.size];

	int *output=new int[graph.size];
	int *dist=new int[graph.size];

	unsigned char *result=0;

	//test the time cost of Dijkstra based on binary heap
	/*unsigned char *path=new unsigned char[graph.size];
	int *dist=new int[graph.size];
	int *label=new int[graph.size];
	double sum=0;
	QueryPerformanceCounter( &begin );
	for(int i=0;i<N;i++)
	{
		s=rand()%(graph.size-1)+1;
		
		graph.Dijkstra(s,path,label,dist);
		
		sum+=totaltime;
		printf("%d\n",i);
	}
	QueryPerformanceCounter( &end );
	totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
	printf("average time for Dijkstra %fus\n",totaltime/N);
	system("pause");
	return 0;*/

	QueryPerformanceCounter( &begin );
	for(int i=0;i<N;i++)
	{
		s=rand()%(graph.size-1)+1;	
		result=graph.regions[graph.nodes[s].id_region].data->GetRecord(buf,graph.nodes[s].id_in_region,false,n_steps);
		for(int i=1;i<N;i++)
			output[i]=graph.nodes[i].arcs[result[i]].id_to;
		output[s]=0;
		//memset(dist,0,sizeof(int)*graph.size);
		//for(int i=1;i<N;i++)
		//{
		//	int parent=output[i];
		//	while(dist[parent]
		//}
	}
	QueryPerformanceCounter( &end );
	totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
	printf("average time for SPLZ %fus\n",totaltime/(1*N));

	system("pause");
}
