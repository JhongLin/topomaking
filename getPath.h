#include <iostream>
#include <vector>
//#include <pair>

using namespace std;


/*
node::node(int _id){
	id = _id;
}
*/

class nodeLink{
public:
	void assign(int id1, int id2, float _cost);
	int ends[2];
	float cost;
};

class node{
public:
        //node(int _id);
        int id;
        int bytes;
        node* next;
        float cost;
        vector < nodeLink > links;
};


class getPath{
public:
	getPath();
	getPath(char* fileName);
	~getPath();

	vector < nodeLink > links;
	//nodeLink links[170];
	vector < node > nodes;
	//node* nodes[170];
	int link_count;
	vector < int > done;
	//int done[170];
	int done_count;
	vector < int > horizon;
	//int horizon[170];
	int horizon_count;

	node* getNode(int ingress, int egress);
	vector < node * > getNode2(int ingress, vector < int > egress);
	vector < node * > getNode(int ingress, vector < int > egress);
	void getDone(int id);
	float getCost(int id1, int id2);
	void modCost(int id1, int id2, float cost);
	void removeHorizon(int id);
	bool checkExist(int id);
	void cutTree(int** tree, int size);
	vector < pair < int, int > > spanningTree();
	//void schedule();
private:
	int** P;
        //int** B;
        int P_size, B_size;
	int rate[3], rate_count[3];
};
