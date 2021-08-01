#include "topo.h"

#include <iostream>
#include <fstream>

#include <string.h>
#include <stdlib.h>

#define SHOW_TREE 0
#define SHOW_NODE 25
#define DEBUG 0

#define BETA 0.3

#define ALPHA 1.5
#define OURS 1
#define REDUCE 2
#define ORIGIN 3

class algo{
public:
	algo(int _mode);
	void run();
	void run_random();
	void evaluation();
	void output_flow_pass_through(char* filename);
	void node_flow_count();
private:
	map < int, vector < int > > getPt(map < int, vector < int > > input_pt);
	bool combinePt(int to_node, int from_node);

	vector < int > sources;
	vector < int > dsts;
	map < int, int > flow_cost;

	map < int, vector < int > > cand_pt;

	map < int, vector < int > > flow_pass_through;//前面為flowid,後者為經過的switch陣列
	map< int,int > node_flow_total; //node點上的經過的flow數



	void readFlow();
	topo myTopo;
	int mode;
};

