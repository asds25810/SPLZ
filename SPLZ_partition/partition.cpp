#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <io.h>
#include <math.h>
#include "..\include\Graph.h"

int main(int argc, char **argv)
{
	if(argc!=5)
	{
		printf("Error: partition needs 5 parameters.\n");
		return 0;
	}
	int C=atoi(argv[2]);
	if(C<=0)
	{
		printf("Error: parameter C must be a positive integer.\n");
		return 0;
	}
	int len_to_dict=atoi(argv[3]);
	if(len_to_dict<=0)
		len_to_dict=1000000;
	STEP=len_to_dict;

	Graph graph;
	if(!graph.LoadGraph(argv[0]))
	{
		printf("Error: fail to load graph data.\n");
		return 0;
	}
	if(!graph.LoadCoordinate(argv[1]))
	{
		printf("Error: fail to load coordinate data.\n");
		return 0;
	}
	graph.Segment((int)sqrt((double)graph.size)*C,25,argv[4]);	
	return 0;
}
