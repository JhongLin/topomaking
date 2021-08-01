#include "algo.h"


algo::algo(int _mode){
	mode = _mode;
	string topoName("output.txt");
	myTopo = topoName;

	if(mode == SVC) myTopo.setNodeCostConsider(false);

	current_flow = -1;
	total_bw = 0;
	total_cost = 0;

	readTree();
}


void algo::readTree(){
	char data[1024];
	int count = 0;

	ifstream inputFile("tree.txt", ifstream::in);
	while(!inputFile.eof()){
		inputFile.getline(data, 1024);
		char* token;
		double total_cost = 0.0;
		int source;
		map < int, bool > dst;
		int stream_num;
		int tree_index = sources.size();

		token = strtok(data, "\t");


		if(token){
			stream_num = atoi(token);
			if(mode == SVC && mode == SDN_CAST_PART){
				stream_num = count;
				count++;
			}
			if(streams.find(stream_num) == streams.end()){
				vector < int > new_stream;
				streams[stream_num] = new_stream;
			}
			streams[stream_num].push_back(tree_index);
		}
		else break;

		token = strtok(NULL, "\t");

		if(token){
			required_bw[tree_index] = atoi(token);
		}
		else break;


		token = strtok(NULL, "\t");

		if(token){
			source = atoi(token);
			sources.push_back(source);
		}
		else break;

		token = strtok(NULL, "\t");
		while(token){
			int node = atoi(token);
			//input_dst[node] = true;
			dst[node] = true;
			token = strtok(NULL, "\t");
		}

		dsts.push_back(dst);
	}

	inputFile.close();	
}

pair < vector < int >, double > algo::distance_to_tree(int fromNode, map < int, treeNode > tree){
	map < int, bool > _dst;
	map < int, treeNode > spanning;
	pair < vector < int >, double > ret;

	ret.second  = -1.0;
	int minNode;

	for(map < int, treeNode >::iterator it = tree.begin(); it != tree.end(); ++it){
		_dst[it->first] = true;
	}

	spanning = myTopo.spanning_tree(fromNode, _dst);

	for(map < int, treeNode >::iterator it = spanning.begin(); it != spanning.end(); ++it){
		if(ret.second < 0 || it->second.cost < ret.second){
			minNode = it->second.id;
			ret.second = it->second.cost;
		}
	}

	int tempNode = minNode;
	while(tempNode != fromNode){
		ret.first.insert(ret.first.begin(), tempNode);
		tempNode = spanning[tempNode].parent;
	}

	ret.first.insert(ret.first.begin(), fromNode);

	return ret;
}


//void algo::add_dst( vector < int > dst){

map < int, bool > algo::scan_dst(map < int, treeNode > tree, int root){
	map < int, bool > ret;


	//if(dst.find(root) != dst.end()) 
	ret[root] = true;

	for(int i = 0; i < tree[root].child.size(); i++){
		map < int, bool > fromChild = scan_dst(tree, tree[root].child[i]);

		if(fromChild.size() > 0)
			ret.insert(fromChild.begin(), fromChild.end());
	}

	return ret;
}

void algo::dump_tree(map < int, treeNode > tree, int root, int level){

	cout << "check 1-1" << endl;

	if(level == 0 && DEBUG)
		cout << "====== Print Tree ======" << endl;

	for(int i = 0; i < level && DEBUG; i++){
		cout << "-";
	}

	if(DEBUG) cout << tree[root].id << endl;

	if(current_flow >= 0 && tree[root].child.empty()){
		if(dsts[current_flow].find(tree[root].id) != dsts[current_flow].end()){
			total_bw += required_bw[current_flow];
		}
	}

	cout << "check 1-2 current flow = " << current_flow << " root = " << tree[root].child.size() << endl;

	for(int i = 0; i < tree[root].child.size(); i++){
		cout << "check 1-3" << endl;
		total_cost += required_bw[current_flow];
		cout << "check 1-4 child = " << tree[root].child[i] << endl;
		dump_tree(tree, tree[root].child[i], level + 1);
	}

	cout << "check 1-5" << endl;
}


void algo::run(){
	vector < int > reroute;	

	for(map < int, vector < int > >::iterator it = streams.begin(); it != streams.end(); ++it){
		rules.clear();
		for(int i  = 0; i < it->second.size(); i++){
			int flow_id = it->second[i];
			map < int, treeNode > new_tree = myTopo.spanning_tree(sources[flow_id], dsts[flow_id]);
			new_tree = cut_tree(new_tree, sources[flow_id], required_bw[flow_id]);
			trees[flow_id] = new_tree;
			if(new_tree.empty()){
				if(DEBUG) cout << "The tree is empty" << endl;
				reroute.push_back(flow_id);
			}
			else{

				current_flow = flow_id;
				//dump_tree(new_tree, sources[flow_id],0);
			}
		}

		for(map < int, vector < vector < int > > >::iterator it = rules.begin(); it != rules.end(); ++it){
			myTopo.nodes[it->first].remaining -= it->second.size();	
		}
	}


	for(int i = 0; i < reroute.size() && mode == SDN_CAST; i++){
		rules.clear();
		int flow_id = reroute[i];

		map < int, treeNode > new_tree = myTopo.spanning_tree(sources[flow_id], dsts[flow_id]);
		new_tree = cut_tree(new_tree, sources[flow_id], required_bw[flow_id]);
		trees[flow_id] = new_tree;
		if(new_tree.empty()){
			if(DEBUG) cout << "The tree is empty" << endl;
		}
		else{
			dump_tree(new_tree, sources[flow_id],0);
		}

		for(map < int, vector < vector < int > > >::iterator it = rules.begin(); it != rules.end(); ++it){
			myTopo.nodes[it->first].remaining -= it->second.size();
		}
	}
}

map < int, treeNode > algo::cut_tree(map < int, treeNode > tree, int root, double bw){
	int nodeID = tree[root].id;
	treeNode newNode = tree[root];
	double nodeCap = myTopo.nodes[nodeID].capacity;
	map < int, treeNode > ret;

	newNode.child.clear();

	
	for(int i = 0; i < tree[root].child.size(); i++){
		int childID = tree[root].child[i];
		int linkCap;
		
		for(int j = 0; j < myTopo.nodes[nodeID].link_id.size(); j++){
			int linkID = myTopo.nodes[nodeID].link_id[j];
			if((myTopo.links[linkID].ends[0] == nodeID && myTopo.links[linkID].ends[1] == childID)
				|| (myTopo.links[linkID].ends[1] == nodeID && myTopo.links[linkID].ends[0] == childID)){
				if(myTopo.links[linkID].remaining >= bw){
					map < int, treeNode > subTree = cut_tree(tree, tree[root].child[i], bw);
					if(!subTree.empty()){
						for(map < int, treeNode >::iterator it = subTree.begin(); it != subTree.end(); ++it){
							ret.insert(*it);
						}
						myTopo.links[linkID].remaining -= bw;

						if(mode != SVC)
							myTopo.links[linkID].cost = 1.0 / myTopo.links[linkID].remaining;

						if(myTopo.links[linkID].ends[0] == nodeID)
							newNode.child.push_back(myTopo.links[linkID].ends[1]);
						else
							newNode.child.push_back(myTopo.links[linkID].ends[0]);
					}
				}

			}
		}

	}

	if(myTopo.nodes[nodeID].remaining < 1){
		ret.clear();
		return ret;
	}



	if(!ret.empty() || tree[root].child.empty()){
		ret[nodeID] = newNode;
		if(!tree[root].child.empty()){
			bool match  = false;
			

			if(rules.find(nodeID) == rules.end()){
				vector < vector < int > > new_rule;
				rules[nodeID] = new_rule;
			}
			
			for(int i = 0; i < rules[nodeID].size(); i++){
				if(rules[nodeID][i] == newNode.child) match = true;
			}

			if(!match) rules[nodeID].push_back(newNode.child);

			if(rules[nodeID].size() > myTopo.nodes[nodeID].remaining){


				int rule_index = rules[nodeID].size() - 1;
				bool bw_ok = true;

				if(rules[nodeID].size() < 2 || mode == SDN_CAST_PART){
					rules[nodeID].erase(rules[nodeID].begin() + rules[nodeID].size() - 1);
					ret.clear();
					return ret;
				}


				vector < int > extend_nodes = rule_diff(rules[nodeID][rule_index - 1], rules[nodeID][rule_index]);

				/*
				for(int i = 0; i < extend_nodes.size(); i++){
					if(myTopo.getRemaining(nodeID, extend_nodes[i]) < bw) bw_ok = false;
					map < int, treeNode > subTree = cut_tree(tree, extend_nodes[i], bw);
					if(subTree.empty()) bw_ok = false;
				}

				if(!bw_ok){
					ret.clear();
					return ret;
				}
				*/

				for(int i = 0; i < extend_nodes.size(); i++){
					treeNode dummy;
					dummy.id = extend_nodes[i];
					dummy.parent = nodeID;
					dummy.is_dest = true;

					tree[extend_nodes[i]] = dummy;

					ret[nodeID].child.push_back(extend_nodes[i]);
					ret[extend_nodes[i]] = dummy;

					if(myTopo.getRemaining(nodeID, extend_nodes[i]) < bw){
						rules[nodeID].erase(rules[nodeID].begin() + rules[nodeID].size() - 1);
						ret.clear();
						return ret;
					}

					/*
					cout << "Extend node = " << extend_nodes[i] << endl;
					map < int, treeNode > subTree = cut_tree(tree, extend_nodes[i], bw);
					if(subTree.empty()){
						rules[nodeID].erase(rules[nodeID].begin() + rules[nodeID].size() - 1);
						ret.clear();
						return ret;
					}
					*/

				}


				rules[nodeID].erase(rules[nodeID].begin() + rules[nodeID].size() - 1);

				/*
				int min_dst = -1;
				int min_rule;
				for(int i = 0; i < rules[nodeID].size() - 1; i++){
					int dst = rules[nodeID][i].size() - rules[nodeID][i+1].size();
					if(min_dst == -1 || min_dst > dst){
						min_dst = dst;
						min_rule = i;
					}
				}

				vector 
				*/
			}

			if(DEBUG) cout << " node " << nodeID << " num of rules = " << rules[nodeID].size() << " remaining = " << myTopo.nodes[nodeID].remaining << endl;
		}
	}

	return ret;
}

vector < int > algo::rule_diff(vector < int > first, vector < int > second){
	vector < int > ret;

	for(int i = 0; i < first.size(); i++){
		bool match = false;
		for(int j = 0; j < second.size(); j++){
			if(first[i] == second[j]){
				match = true;
				break;
			}
		}
		if(!match) ret.push_back(first[i]);
	}

	return ret;
}

void algo::evaluation(){
	for(map < int, map <int, treeNode > >::iterator it = trees.begin(); it != trees.end(); ++it){
		current_flow = it->first;
		dump_tree(it->second, sources[current_flow], 0);
	}

	cout << "Total throughput = " << total_bw << endl;
	cout << "Total cost = " << total_cost << endl;
	cout << "Unit cost = " << ((double) total_cost / (double) total_bw) << endl;
}
		

int main(int argc, char* argv[]){

	if(argc < 2){
		cout << "Usage: ./algo <mode>" << endl;
		return 1;
	}

	int mode = atoi(argv[1]);

	algo myAlgo(mode);
	myAlgo.run();
	myAlgo.evaluation();
	//algo myAlgo_rand(mode);
}
