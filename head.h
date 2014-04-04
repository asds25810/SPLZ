#pragma once

#include <hash_set>
//using namespace stdext;
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

#include <math.h>
#define STEP 1000000

void PreProcess(int k);
void PreProcess();
void Reorder();
struct arc
{
    int id_to;
	int weight;
	int importance;
};
struct vertex
{
    int id;
	int degree;//出度
	int id_region;//属于哪个region
	int id_in_region;//region内部编号
	int importance;
	int x;
	int y;
    arc *arcs;
	bool operator < (const vertex &m)const
	{
		return this->importance > m.importance;
    }
	/*bool operator > (const vertex &m)const
	{
		return this->importance > m.importance;
    }*/
};
double global_time=0;
bool begin_to_record=false;
#include "DataBlock.h"
struct region
{
	int id;
	int size;
	vector<int> nodes;
	DataBlock *data;
};

class Graph
{
	public:
		int n_regions;
		int size;//结点数
		int n_arc;
		vertex *nodes;//结点集
		region *regions;
		int x_west;
		int x_east;
		int y_north;
		int y_south;
		int count_current;//当前记录数
		int size_current;//当前压缩数据总大小  按sizeof(int)计
		Graph();
		Graph(char *filepath);
		void LoadGraph(char *filepath);
		void LoadRegion(char *filename);
		void LoadCoordinate(char *filename);
		void Segment(int k);
		void Segment2(int k);
		void Segment3(int k,int n);
		double evaluate();
		void Dijkstra(int s,int *path,int *label);
		void Dijkstra(int source,unsigned char *path,int *label,int *dist);
		void BFM(int source,unsigned char *path,int *label);
		void Get_Importance();
		void Compress(char *dirpath,int a,int b);
		void Decompress_prepare(char *dirpath,bool exMemory);
		void Reorder(char *filename);
		int Astar(int s,int t,int *path,int *label,double epsilon);
};
