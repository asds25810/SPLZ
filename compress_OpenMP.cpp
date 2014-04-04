#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/io.h>
#include <math.h>
#include <omp.h>
#include "Graph.h"

int main(int argc,char **argv)
{
	//char *file_gr="E:\\GraphData\\USA-road-d.NW.gr";
	//char *file_regions="E:\\GraphData\\regions_Kmeans_2.txt";

	Graph graph;

		//读取数据文件
	graph.LoadGraph("USA-road-d.NW.gr");
	//graph.LoadCoordinate("E:\\GraphData\\USA-road-d.NW.co");
	graph.LoadRegion("regions_Kmeans_2.txt");
	
#pragma omp parallel for schedule(dynamic)
	//graph.Compress("E:\\regions_Kmeans\\",1,1+1);
	for(int i=0;i<graph.n_regions;i++)		
	{
		graph.Compress("regions_Kmeans_2_64\\",i,i+1);
	}

}


	
