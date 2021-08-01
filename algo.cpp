#include "algo.h"
# include <iostream>
using namespace std;

algo::algo(int _mode){

	//cout << "Check 1" << endl;

	
	mode = _mode;
	string topoName("output.txt");
	myTopo = topoName;

	readFlow();
}
void algo::output_flow_pass_through(char* filename){
	//give the name of output file
	FILE* output = fopen(filename,"w");
	
	for(map < int, vector < int > >::iterator it = flow_pass_through.begin();
		it !=flow_pass_through.end() ; ++it){
		// print flow id & total number of nodes of each flow
		fprintf(output, "%d\t%d\t", it->first, it->second.size());
		//cout << "Flow " << it->first << ",size" << it->second.size() << endl;
		//cout << "Node:";
		// print all the nodes of each flow
		for(int i=0;i<it->second.size();i++){
			fprintf(output, "%d\t", it->second[i]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
	
}
void algo::node_flow_count(){
	int totalnode = 0;

	for(map < int, vector < int > >::iterator it = flow_pass_through.begin();
		it !=flow_pass_through.end() ; ++it){
		for(int i =0;i<it->second.size();i++){
			if (node_flow_total.find(it->second[i]) == node_flow_total.end()){
				node_flow_total.insert(make_pair(it->second[i],1));
			}else{
				node_flow_total[it->second[i]]++;
			}
		}

	}
	for (auto it = node_flow_total.begin(); it !=node_flow_total.end();it++){
		cout << it->first << " " << it->second  << endl;
		totalnode += it->second;
	}
	cout << "Total nodes : " << totalnode << endl;
	cout << "Average nodes : " << totalnode / 10 << endl;
}


void algo::readFlow(){
	char data[1024];
	int count = 0;
	int src_num;
	int dst_num;

	ifstream inputFile("flow.txt", ifstream::in); //getflow.pl產生之flow.txt  
	while(!inputFile.eof()){
		inputFile.getline(data, 1024);
		char* token;

		token = strtok(data, "\t"); //切割字串


		if(token){
			src_num = atoi(token);
		}
		else break; 

		token = strtok(NULL, "\t");

		if(token){
			dst_num = atoi(token);
                }
                else break;

		sources.push_back(src_num);//新增元素至 vector 的尾端，必要時會進行記憶體配置。
		dsts.push_back(dst_num);
		//可以從flow.txt 知道哪些flow是從哪個src_switch 流到dst_switch （可能跟output有bug）
	}

	inputFile.close();	
}

void algo::run_random(){
	for(int i = 0; i < sources.size(); i++){
		int src_num = sources[i];//來源switch
		int dst_num = dsts[i];//目的switch
	    
		//pair的意思就是把兩個物件裝在一起的東西 int-：path->id ; double-：path->cost 
		vector < pair < int, double > > sp = myTopo.shortest_path(src_num, dst_num);
		//sp去紀錄shortest_path的每個點及成本
		//sp存著shortest path每個點的[switch,cost],結構問題：局部解！＝最佳解
		
		for(int j=0;j<sp.size();j++){
			flow_pass_through[i].push_back(sp[j].first);
		}

		//no use in minglin code (for making monitoring point)
		flow_cost[i] = sp.size() - 1; //flowcost = shortestpath經過的switch數,sp存的包含本身自己,所以-1
		int pt = rand() % sp.size();
		pt = sp[pt].first;//隨機從shortest path挑出點當作監控點
		cand_pt[pt].push_back(i);//紀錄監控的switch有哪些flow經過
	}
		/**********************計算出each flow經過哪些節點*************************************
		cout << "\n";
	    for(map < int, vector < int > >::iterator it = flow_pass_through.begin();it !=flow_pass_through.end() ; ++it){
		cout << "Flow " << it->first << ",size" << it->second.size() << endl;
		cout << "Node:";
		for(int i=0;i<it->second.size();i++){
			cout  <<it->second[i]<<"	";
		}
		cout << "\n";
		}
		************************************************************************************/
		output_flow_pass_through("flow_pass_through.txt");
		//count and list all of the node counts
		cout << "Total node counts : " << endl;
		node_flow_count();

	//cout << "the number of pt = " << cand_pt.size() << endl;
	
}

void algo::run(){
	for(int i = 0; i < sources.size(); i++){
		int src_num = sources[i];
		int dst_num = dsts[i];

		vector < pair < int, double > > sp = myTopo.shortest_path(src_num, dst_num);
		
		flow_cost[i] = sp.size() - 1;

		for(int j = 0; j < sp.size(); j++){
			cand_pt[sp[j].first].push_back(i);
		}
	}

	//map < int, vector < int > > pt 
	cand_pt = getPt(cand_pt);
	map < int, vector < int > > tmp_pt = cand_pt;

	while(!tmp_pt.empty() && mode == OURS){	
		int min_cover = 0;
		int min_node;

		for(map < int, vector < int > >::iterator it = tmp_pt.begin(); it != tmp_pt.end(); ++it){
			if(min_cover == 0 || min_cover > it->second.size()){
				min_cover = it->second.size();
				min_node = it->first;
			}
		}

		tmp_pt.erase(min_node);
	
		for(map < int, vector < int > >::iterator it = tmp_pt.begin(); it != tmp_pt.end(); ++it){
			if(combinePt(it->first, min_node)){
				cand_pt[it->first].insert(cand_pt[it->first].end(), cand_pt[min_node].begin(), cand_pt[min_node].end());
				cand_pt.erase(min_node);
				break;
			}
		}
	}
	

	//for(map < int, vector < int > >::iterator it = cand_pt.begin(); it != cand_pt.end(); ++it){
	//	cout << "node " << it->first << ": " << it->second.size() << endl;
	//}

	cout << "the number of pt = " << cand_pt.size() << endl;
}

map < int, vector < int > > algo::getPt(map < int, vector < int > > input_pt){
	map < int, bool > covered;
	map < int, vector < int > > temp_pt = input_pt;
	int max_covered = 0;
	int max_node;
	map < int, vector < int > > ret;

	while(covered.size() < dsts.size()){
		max_covered = 0;

		for(int i = 0; i < temp_pt.size(); i++){
			if(temp_pt[i].size() > max_covered){
				max_covered = temp_pt[i].size();
				max_node = i;
			}
		}

		for(int i = 0; i < temp_pt[max_node].size(); i++){
			if(covered.find(temp_pt[max_node][i]) == covered.end()){
				ret[max_node].push_back(temp_pt[max_node][i]);
				covered[temp_pt[max_node][i]] = true;
			}
		}


		temp_pt.erase(max_node);
	}

	return ret;
}

bool algo::combinePt(int to_node, int from_node){
	vector < int > mod_flows = cand_pt[from_node];

	bool good_to_combine = true;

	for(int i = 0; i < mod_flows.size(); i++){
		int new_cost = 0;
		vector < pair < int, double > > sp = myTopo.shortest_path(sources[mod_flows[i]], to_node);
		new_cost += (sp.size() - 1);
		sp = myTopo.shortest_path(to_node, dsts[mod_flows[i]]);
		new_cost += (sp.size() - 1);

		if(new_cost > (flow_cost[mod_flows[i]] * ALPHA)) good_to_combine = false;
	}

	return good_to_combine;
}


void algo::evaluation(){
}
		

int main(int argc, char* argv[]){


	if(argc < 2){
		cout << "Usage: ./algo <mode>" << endl;
		return 1;
	}

	int mode = atoi(argv[1]);

	switch(mode){
	case OURS:
		cout << "OURS: ";
		break;
	case REDUCE:
		cout << "REDUCE: ";
		break;
	case ORIGIN:
		cout << "MODE : ORIGIN" << endl;
		break;
	default:
		cout << "<< Wroung mode >>" << endl;
		return 1;
	} 
	//pt指的是監控的點數	
	algo myAlgo(mode); //宣告時就去讀flow.txt的檔案，建立出vector source和dst
	if(mode == ORIGIN) myAlgo.run_random();
	else myAlgo.run();
	myAlgo.evaluation();
}
