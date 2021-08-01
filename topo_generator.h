#include <iostream>
#include <vector>

using namespace std;

class nodeUnit{
public:
	nodeUnit(int nodeID_in);
	//nodeUnit* next;
	int nodeID;
};

class linkUnit{
public:
	linkUnit(nodeUnit* end0_in, nodeUnit* end1_in);
	nodeUnit* end0;
	nodeUnit* end1;
	double weight;
};

class groupUnit{
public:
	groupUnit(int nodeID_in);
	vector < nodeUnit *> nodeList;
	groupUnit* next;
};

class topoUnit{
public:	
	topoUnit(int num_of_node_in);
	//groupUnit* getGroup(int index);
	void connectGroup(groupUnit* group1, groupUnit* group2);
	void output(char* fileName);
	bool searchDuplicate(linkUnit* newLink);
	void setLink(int num_of_link);
	vector < groupUnit > groupList;
	vector < linkUnit > linkList;
	int process();
	int num_of_node;
	int num_of_link;
};
