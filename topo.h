#include <iostream>
#include <vector>
#include <map>

using namespace std;

class link{
public:
	void assign(int id1, int id2, double _cost);
	int ends[2];
	double cost;
	double capacity;
	double remaining;
};

class node{
public:
        //node(int _id);
        int id;
        double capacity;
	    double remaining;
        node* next;
        double cost;
        vector < int > link_id;
};

class treeNode{
public:
	int id;
	int parent;
	bool is_dest;
	double cost;
	vector < int > child;
	int deep_down;
};


class topo{
public:
	topo(vector < string > input);
	topo(string plaintext);
	topo();
	map < int, treeNode > build_steiner_tree(int nodeID
        	, map < pair < int, int >, bool > _doneLink, int hops);
	vector < pair < int, double > > shortest_path(int ingress, int egress);
	map < int, treeNode > spanning_tree(int ingress, map < int, bool > egress);
	map < int, treeNode > steiner_tree(int ingress, map < int, bool > egress);
	void dump();
	void dump_tree( map < int, treeNode > tree, int root, int level);
	vector < string > tree_to_link(map < int, treeNode > tree, int root);
	void list_tree_node(map < int, treeNode > tree);
	void algo();
	map < int, bool > spanning_tree_proc(int ingress, double cost_threshold);
	double getCost(int start, int end);
	double getRemaining(int start, int end);
	void setCost(int start, int end, double cost);
	double getTreeCost(map < int, treeNode > tree, int root);
	int getTreeHop(map < int, treeNode > tree, int root);
	int getTotalCap();
	void setNodeCostConsider(bool input);

	vector < node > nodes;
	vector < link > links;

private:
	map < int, bool > done;
	map < int, bool > horizon;
	int num_nodes;
	int total_capacity;
	bool enable_node_cost;

	void getDone(int id);
	void getDone(int id, double cost_threshold);
	node* shortest_path_proc(int ingress, int egress);
	vector < node* > spanning_tree_proc(int ingress, map < int, bool > egress);
	//double getCost(int start, int end);
	void readNodeCap();
};
