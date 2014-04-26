#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <io.h>
#include <math.h>
#include <omp.h>
#include "..\include\Graph.h"

#define N 300000
int main()
{
	LARGE_INTEGER begin,end,lv;
	double totaltime,secondsPerTick;
	QueryPerformanceFrequency(&lv);
	secondsPerTick=1000000.0/lv.QuadPart;
	srand(time(0));

	Graph graph("USA-road-d.NW.gr");
	graph.LoadCoordinate("USA-road-d.NW.co");
	graph.LoadRegion("regions_Kmeans_1_step1.txt");

	QueryPerformanceCounter( &begin );
	graph.Decompress_prepare("regions_Kmeans_1_step1_64\\",false);
	QueryPerformanceCounter( &end );
	totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
	printf("prepare time=%fus\n",totaltime);

	int s=1;
	int n_steps=0;
	unsigned char *buf[2];
	buf[0]=new unsigned char[graph.size];
	buf[1]=new unsigned char[graph.size];


	unsigned char *result=0;

	//test the time cost of Dijkstra based on binary heap
	/*unsigned char *path=new unsigned char[graph.size];
	int *dist=new int[graph.size];
	int *label=new int[graph.size];
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
	*/

	QueryPerformanceCounter( &begin );
	for(int i=0;i<N;i++)
	{
		s=rand()%(graph.size-1)+1;	
		result=graph.regions[graph.nodes[s].id_region].data->GetRecord(buf,graph.nodes[s].id_in_region,false,n_steps);
	}
	QueryPerformanceCounter( &end );
	totaltime = secondsPerTick * (end.QuadPart-begin.QuadPart);
	printf("average time for SPLZ %fus\n",totaltime/(1*N));

	system("pause");
}
