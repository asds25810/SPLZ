#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <io.h>
#include <math.h>
#include <omp.h>
#include "..\include\Graph.h"

int main(int argc,char **argv)
{
	if(argc!=3)
	{
		printf("Error: partition needs 3 parameters.\n");
		return 0;
	}
	Graph graph;
	if(!graph.LoadGraph(argv[0]))
	{
		printf("Error: fail to load graph data.\n");
		return 0;
	}
	if(!graph.LoadRegion(argv[1]))
	{
		printf("Error: fail to load region information.\n");
		return 0;
	}
	
	#pragma omp parallel for schedule(dynamic);
	for(int i=0;i<graph.n_regions;i++)		
	{
		graph.Compress(argv[2],i,i+1);
	}

}


	
