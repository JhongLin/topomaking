#include "topo.h"
#include <fstream>

#include <string.h>
#include <stdlib.h>

void link::assign(int id1, int id2, double _cost){
	ends[0] = id1;
	ends[1] = id2;
	capacity = _cost;
	remaining = _cost;
	cost = 1.0 / remaining;
}

topo::topo(){
	topo("output.txt");
}

topo::topo(vector < string > input){
	num_nodes = 0;
	enable_node_cost = true;

	for(int i = 0; i < input.size(); i++){
		int dd;
                int id1;
                int id2;
                double cost;
		sscanf(input[i].c_str(), "%d %d %d %lf\n", &dd, &id1, &id2, &cost);

		if(id1 > num_nodes) num_nodes = id1;
		if(id2 > num_nodes) num_nodes = id2;
	}

	num_nodes++;

	for(int i = 0; i < num_nodes; i++){
                node newNode;
                nodes.push_back(newNode);
        }

	for(int i = 0; i < input.size(); i++){
                int dd;
                int id1;
                int id2;
                double cost;
		int link_id = links.size();
		link newLink;
                sscanf(input[i].c_str(), "%d %d %d %lf\n", &dd, &id1, &id2, &cost);
		newLink.assign(id1, id2, cost);

		nodes[id1].cost = 0.0;
	       	nodes[id1].next = NULL;
        	nodes[id1].id = id1;


		nodes[id1].link_id.push_back(link_id);

                nodes[id2].cost = 0.0;
                nodes[id2].next = NULL;
                nodes[id2].id = id2;
		
                nodes[id2].link_id.push_back(link_id);



		links.push_back(newLink);
                //printf("%d %d %f\n", id1, id2, cost);
        }

	//readNodeCap();
}


topo::topo(string plaintext){
	FILE* input_file = fopen(plaintext.c_str(), "r");
	FILE* node_cap = fopen("nodeCap.txt", "r");
	num_nodes = 0;

        if(!input_file) return;

	while(!feof(input_file)){
		int dd;
                int id1;
                int id2;
                double cost;
		fscanf(input_file, "%d %d %d %lf\n", &dd, &id1, &id2, &cost);

		if(id1 > num_nodes) num_nodes = id1;
		if(id2 > num_nodes) num_nodes = id2;
	}

	num_nodes++;

	for(int i = 0; i < num_nodes; i++){
                node newNode;
                nodes.push_back(newNode);
        }

	fseek(input_file, 0, SEEK_SET);

        while(!feof(input_file)){
                int dd;
                int id1;
                int id2;
                double cost;
		int link_id = links.size();
		link newLink;
                fscanf(input_file, "%d %d %d %lf\n", &dd, &id1, &id2, &cost);
		newLink.assign(id1, id2, cost);

		nodes[id1].cost = 0.0;
	       	nodes[id1].next = NULL;
        	nodes[id1].id = id1;


		nodes[id1].link_id.push_back(link_id);

                nodes[id2].cost = 0.0;
                nodes[id2].next = NULL;
                nodes[id2].id = id2;
		
                nodes[id2].link_id.push_back(link_id);



		links.push_back(newLink);
                //printf("%d %d %f\n", id1, id2, cost);
        }

	while(!feof(node_cap)){
		int nodeID;
		int cap;

		fscanf(node_cap, "%d %d\n", &nodeID, &cap);

		nodes[nodeID].capacity = (double) cap;
		nodes[nodeID].remaining = (double) cap;
	}

        fclose(input_file);
	fclose(node_cap);
	//readNodeCap();
}

void topo::setNodeCostConsider(bool input){
	enable_node_cost = input;
}

vector < pair < int, double > > topo::shortest_path(int ingress, int egress){
	node* path = shortest_path_proc(ingress, egress);
	vector < pair < int, double > > ret;

	//2019.05.22 fix 
	//如果有src跟dst是一樣的話，path會變成NULL，所以這邊加入檢查
	if(ingress == egress){
		pair < int, double > newNode;
		newNode.first = ingress;
		newNode.second = 0;
		ret.push_back(newNode);
		return ret;
	}

	while(path){
		//cout << "check 2  id = " << path->id << endl;
		pair < int, double > newNode;
		newNode.first = path->id;
		newNode.second = path->cost;

		ret.push_back(newNode);
		path = path->next;
	}

	return ret;
}

map < int, treeNode > topo::build_steiner_tree(int nodeID
	, map < pair < int, int >, bool > _doneLink, int hops){

	map < int, treeNode > ret;
	treeNode thisNode;
	thisNode.id = nodeID;
	thisNode.parent = -1;
	thisNode.is_dest = false;

	ret[nodeID] = thisNode;
	hops++;
	nodes[nodeID].cost = hops;


	map < pair < int, int >, bool > doneLink = _doneLink;

	vector < int > link_id = nodes[nodeID].link_id;
	for(int i = 0; i < link_id.size(); i++){
		link link_tmp = links[link_id[i]];
		pair < int, int > local_link;
		local_link.first = link_tmp.ends[0];
		local_link.second = link_tmp.ends[1];
		bool getChild = false;
		if(doneLink.find(local_link) != doneLink.end()){
			doneLink.erase(local_link);
			getChild = true;
		}

		int tmp = local_link.first;
		local_link.first = local_link.second;
		local_link.second = tmp;

		if(doneLink.find(local_link) != doneLink.end()){
			doneLink.erase(local_link);
			getChild = true;
		}

		if(getChild){
			if(local_link.first == nodeID){
				map < int, treeNode > newMap 
					= build_steiner_tree(local_link.second
					, doneLink, hops);
				newMap[local_link.second].parent = nodeID;
				ret[nodeID].child.push_back(local_link.second);
				ret.insert(newMap.begin(), newMap.end());
			}
			else{
				map < int, treeNode > newMap 
					= build_steiner_tree(local_link.first
					, doneLink, hops);
				newMap[local_link.first].parent = nodeID;
				ret[nodeID].child.push_back(local_link.first);
				ret.insert(newMap.begin(), newMap.end());
			}

		}
	}

	return ret;
}

map < int, treeNode > topo::steiner_tree(int ingress, map < int, bool > egress){
	map < int, treeNode > ret;
	map < int, bool > allNodes = egress;
	allNodes[ingress] = true;

	map < pair < int, int >, bool > doneLink;
	map < int, bool > doneNode;

	double minCost = -1.0;
	pair < int, int > minPair;
	vector < pair < int, int > > minPath;

	for(map < int, bool >::iterator it = allNodes.begin(); it != allNodes.end(); ++it){
		map < int, bool > local_dst = allNodes;
		local_dst.erase(it->first);
		vector < node* > span = spanning_tree_proc(it->first, local_dst);

		for(int i = 0; i < span.size(); i++){
			if(minCost < 0 || span[i]->cost < minCost){
				minCost = span[i]->cost;
				minPath.clear();

				minPair.first = it->first;
				minPair.second = span[i]->id;
				
				node* temp = span[i];
				while(temp && temp->next){
					pair < int, int > newPair;
					newPair.first = temp->id;
					newPair.second = temp->next->id;
					minPath.push_back(newPair);
					temp = temp->next;
				}
			}
		}
	}

	for(int i = 0; i < minPath.size(); i++){
		doneLink[minPath[i]] = true;
		doneNode[minPath[i].first] = true;
		doneNode[minPath[i].second] = true;

		allNodes.erase(minPair.first);
		allNodes.erase(minPair.second);
	}

	while(allNodes.size() > 0){
		int minCost = -1.0;
		for(map < int, bool >::iterator it = allNodes.begin(); it != allNodes.end(); ++it){
			vector < node* > span = spanning_tree_proc(it->first, doneNode);
			
			for(int i = 0; i < span.size(); i++){
				if(minCost < 0 || span[i]->cost < minCost){
					minCost = span[i]->cost;
					minPath.clear();

					minPair.first = it->first;
					minPair.second = span[i]->id;
				
					node* temp = span[i];
					while(temp && temp->next){
						pair < int, int > newPair;
						newPair.first = temp->id;
						newPair.second = temp->next->id;
						minPath.push_back(newPair);
						temp = temp->next;
					}
				}
			}
		}

		for(int i = 0; i < minPath.size(); i++){
			doneLink[minPath[i]] = true;
			doneNode[minPath[i].first] = true;
			doneNode[minPath[i].second] = true;

			allNodes.erase(minPair.first);
			allNodes.erase(minPair.second);
		}
	}

	return build_steiner_tree(ingress, doneLink, -1);
}

map < int, treeNode > topo::spanning_tree(int ingress, map < int, bool > egress){
	vector < node* > path = spanning_tree_proc(ingress, egress);
	map < int, treeNode > ret;


	for(int i = 0; i < path.size(); i++){
		node* temp = path[i];
		int prev_id = -1;

		while(temp){

			if(ret.find(temp->id) == ret.end()){
				treeNode newNode;
				newNode.id = temp->id;
				newNode.parent = -1;

				if(egress.find(temp->id) != egress.end()) newNode.is_dest = true;
				else newNode.is_dest = false;

				ret[temp->id] = newNode;
				ret[temp->id].cost = temp->cost;
			}

			if(prev_id >= 0){
				if(ret.find(prev_id) != ret.end()){
					bool got = false;
					ret[prev_id].parent = temp->id;
					for(int j = 0; j < ret[temp->id].child.size(); j++){
						if(ret[temp->id].child[j] == prev_id){
							got = true;
							break;
						}
					}

					ret[temp->id].cost = temp->cost;

					if(!got)
						ret[temp->id].child.push_back(prev_id);
				}
			}

			prev_id = temp->id;
			temp = temp->next;
		}
	}

	return ret;
}
			
 

vector < node* > topo::spanning_tree_proc(int ingress, map < int, bool > egress){
	nodes[ingress].next = NULL;
	nodes[ingress].cost = 0.0;

	vector < node* > ret;

	done.clear();
	horizon.clear();

	int processID = ingress;
	int count = 0;
	do{
		//cout << "check 1 id = " << processID << endl;

		getDone(processID);
		double minCost = -1.0;
		for(map< int , bool >::iterator it=horizon.begin(); it!=horizon.end(); ++it){
			if(minCost == -1 || minCost > nodes[it->first].cost){
				minCost = nodes[it->first].cost;
				processID = it->first;
			}
			
		}

		if(egress.find(processID) != egress.end()){
			ret.push_back(&nodes[processID]);
			egress.erase(processID);
		}


		count++;
	} while(horizon.size() != 0 && egress.size() > 0);
	return ret;
}

map < int, bool > topo::spanning_tree_proc(int ingress, double cost_threshold){
	nodes[ingress].next = NULL;
	nodes[ingress].cost = 0.0;

	vector < node* > ret;

	done.clear();
	horizon.clear();

	int processID = ingress;
	int count = 0;
	do{
		//cout << "check 1 id = " << processID << endl;

		getDone(processID, cost_threshold);
		double minCost = -1.0;
		for(map< int , bool >::iterator it=horizon.begin(); it!=horizon.end(); ++it){
			if(minCost == -1 || minCost > nodes[it->first].cost){
				minCost = nodes[it->first].cost;
				processID = it->first;
			}
			
		}

		/*
		if(egress.find(processID) != egress.end()){
			ret.push_back(&nodes[processID]);
			egress.erase(processID);
		}
		*/

		count++;
	} while(horizon.size() != 0);
	return done;
}

node* topo::shortest_path_proc(int ingress, int egress){

	nodes[ingress].next = NULL;
	nodes[ingress].cost = 0.0;

	done.clear();
	horizon.clear();

	int processID = ingress;
	int count = 0;
	do{
		getDone(processID);//processID==ingress
		float minCost = -1.0;
		for(map< int , bool >::iterator it=horizon.begin(); it!=horizon.end(); ++it){
			if(minCost == -1 || minCost > nodes[it->first].cost){
				minCost = nodes[it->first].cost;
				processID = it->first;
			}
			
		}
		if(processID == egress){
			return &nodes[processID];
		}


		count++;
	} while(horizon.size() != 0);
	return NULL;
}

void topo::getDone(int id){ //id==ingress
	horizon.erase(id);
	done[id] = true;

	for(map < int , bool >::iterator it=horizon.begin(); it!=horizon.end(); ++it){
		int link_cost = getCost(id, it->first);
		if(link_cost == -1.0) continue;
		if(nodes[it->first].cost > nodes[id].cost + link_cost){
			nodes[it->first].cost = nodes[id].cost + link_cost;
			nodes[it->first].next = &nodes[id];
		}
	}


	for(int i = 0; i < nodes[id].link_id.size(); i++){
	//for(int i = 0; i < link_count; i++){
		int new_ID = -1;
		double cost = 0.0;


		if(links[nodes[id].link_id[i]].ends[0] == id){

			new_ID = links[nodes[id].link_id[i]].ends[1];
			cost = links[nodes[id].link_id[i]].cost;
			if(enable_node_cost) cost += 1.0 / nodes[id].remaining;

		}
		else if(links[nodes[id].link_id[i]].ends[1] == id){
			new_ID = links[nodes[id].link_id[i]].ends[0];
			cost = links[nodes[id].link_id[i]].cost;

			if(enable_node_cost) cost += 1.0 / nodes[id].remaining;
		}

		if(new_ID != -1 && horizon.find(new_ID) == horizon.end() 
			&& done.find(new_ID) == done.end()){


			horizon[new_ID] = true;
			//nodes[new_ID] = new node();
			nodes[new_ID].cost = cost + nodes[id].cost;
			nodes[new_ID].next = &nodes[id];
			//nodes[new_ID]->id = new_ID;
		}
	}
}

void topo::getDone(int id, double cost_threshold){
	horizon.erase(id);
	done[id] = true;

	for(map < int , bool >::iterator it=horizon.begin(); it!=horizon.end(); ++it){
		int link_cost = getCost(id, it->first);
		if(link_cost == -1.0) continue;
		if(nodes[it->first].cost > nodes[id].cost + link_cost){
			nodes[it->first].cost = nodes[id].cost + link_cost;
			nodes[it->first].next = &nodes[id];
		}
	}


	for(int i = 0; i < nodes[id].link_id.size(); i++){
	//for(int i = 0; i < link_count; i++){
		int new_ID = -1;
		float cost = 0.0;


		if(links[nodes[id].link_id[i]].ends[0] == id){

			new_ID = links[nodes[id].link_id[i]].ends[1];
			cost = links[nodes[id].link_id[i]].cost;


		}
		else if(links[nodes[id].link_id[i]].ends[1] == id){
			new_ID = links[nodes[id].link_id[i]].ends[0];
			cost = links[nodes[id].link_id[i]].cost;
		}

		if(new_ID != -1 && horizon.find(new_ID) == horizon.end() 
			&& done.find(new_ID) == done.end() 
			&& (cost + nodes[id].cost) <= cost_threshold){


			horizon[new_ID] = true;
			//nodes[new_ID] = new node();
			nodes[new_ID].cost = cost + nodes[id].cost;
			nodes[new_ID].next = &nodes[id];
			//nodes[new_ID]->id = new_ID;
		}
	}
}


double topo::getCost(int start, int end){
	for(int i = 0; i < nodes[start].link_id.size(); i++){
		if(links[nodes[start].link_id[i]].ends[0] == end
			|| links[nodes[start].link_id[i]].ends[1] == end)
				return links[nodes[start].link_id[i]].cost;
	}

	return -1.0;
}

double topo::getRemaining(int start, int end){
	for(int i = 0; i < nodes[start].link_id.size(); i++){
		if(links[nodes[start].link_id[i]].ends[0] == end
			|| links[nodes[start].link_id[i]].ends[1] == end)
				return links[nodes[start].link_id[i]].remaining;
	}

	return -1.0;
}


void topo::setCost(int start, int end, double cost){
	for(int i = 0; i < nodes[start].link_id.size(); i++){
		if(links[nodes[start].link_id[i]].ends[0] == end
			|| links[nodes[start].link_id[i]].ends[1] == end)
				links[nodes[start].link_id[i]].cost = cost;
	}

}


void topo::readNodeCap(){
	FILE* input_file = fopen("nodeCapacity.txt", "r");

	total_capacity = 0;	
	while(!feof(input_file)){
		int nodeID;
		int nodeCap;
		fscanf(input_file, "%d\t%d", &nodeID, &nodeCap);
	
		total_capacity += nodeCap;
		nodes[nodeID].capacity = nodeCap;
		nodes[nodeID].remaining = nodeCap;
	}

	fclose(input_file);
}

int topo::getTotalCap(){
	return total_capacity;
}


void topo::dump(){
	cout << "nodes:" << endl;

	for(int i = 0; i < nodes.size(); i++){
		cout << "node " << i << ":" << endl;
		for(int j = 0; j < nodes[i].link_id.size(); j++){
			cout << "  " << links[nodes[i].link_id[j]].ends[0] << " --> "
				<< links[nodes[i].link_id[j]].ends[1] << endl;
		}
	}
}

void topo::dump_tree( map < int, treeNode > tree, int root, int level){
	for(int i = 0 ; i < level; i++){
		cout << "  ";
	}

	cout << "- " << root << endl;

	for(int i = 0; i < tree[root].child.size(); i++){
		dump_tree(tree, tree[root].child[i], level + 1);
	}
}

vector < string > topo::tree_to_link(map < int, treeNode > tree, int root){
	vector < string > ret;

	for(int i = 0; i < tree[root].child.size(); i++){
		char newLink_str[128];
		memset(newLink_str, 0, 128);

		sprintf(newLink_str, "%d %d %d 1.0\n", 0, root, tree[root].child[i]);
		string newLink(newLink_str);
		ret.push_back(newLink);

		vector < string > fromChild = tree_to_link(tree, tree[root].child[i]);
		ret.insert(ret.begin(), fromChild.begin(), fromChild.end());
	}

	return ret;
}


void topo::list_tree_node(map < int, treeNode > tree){
	for(map < int, treeNode >::iterator it = tree.begin(); it != tree.end(); ++it){
		cout << it->first << "\t";
	}

	cout << endl;
}

double topo::getTreeCost(map < int, treeNode > tree, int root){
	double ret = 0.0;

	if(tree[root].parent >= 0){
		ret += getCost(root, tree[root].parent);
	}

	for(int i = 0; i < tree[root].child.size(); i++){
		ret += getTreeCost(tree, tree[root].child[i]);
	}

	return ret;
}

int topo::getTreeHop(map < int, treeNode > tree, int root){
	double ret = 0.0;

	if(tree[root].parent >= 0){
		ret += 1; //getCost(root, tree[root].parent);
	}


	for(int i = 0; i < tree[root].child.size(); i++){
		ret += getTreeCost(tree, tree[root].child[i]);
	}


	return ret;
}
