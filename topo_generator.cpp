#include <stdio.h>
#include <stdlib.h>
#include "topo_generator.h"
#include "getPath.h"

nodeUnit::nodeUnit(int nodeID_in){
	nodeID = nodeID_in;
	//next = NULL;
}

linkUnit::linkUnit(nodeUnit* end0_in, nodeUnit* end1_in){
	end0 = end0_in;
	end1 = end1_in;
	weight = (double) (rand() % 1000000) / 10000.0;
}

groupUnit::groupUnit(int nodeID_in){
	nodeUnit* firstNode = new nodeUnit(nodeID_in);
	nodeList.push_back(firstNode);
}

topoUnit::topoUnit(int num_of_node_in){
	num_of_node = num_of_node_in;

	for(int i = 0; i < num_of_node_in; i++){
		groupUnit newGroup(i);
		groupList.push_back(newGroup);//將節點各自當成一個group並加到grouplist裡 
	}
}

int topoUnit::process(){
	while(groupList.size() > 1){ //當合併城只有一個group時就結束
		int index_group1 = 0;
		int index_group2 = 0;

		while(index_group1 == index_group2){
			index_group1 = rand() % groupList.size();
			index_group2 = rand() % groupList.size();
		} //用random的方式將600個node當作600個group,利用隨機方式最後合併成一個group

		connectGroup(&groupList[index_group1], &groupList[index_group2]);
		//delete groupList[index_group2];
		groupList.erase(groupList.begin() + index_group2);
	}
}

/*
groupUnit* topoUnit::getGroup(int index){
	groupUnit* retGroup;
	if(num_of_node <= index || index < 0) return NULL;
	if(index == 0){
		retGroup = groupList;
		groupList = groupList->next;
		return retGroup;
	}

	int count = index - 1;
	groupUnit* tempGroup = groupList;
	for(int i = 0; i < count; i++)
		tempGroup = tempGroup->next;
	
	retGroup = tempGroup->next;
	tempGroup = retGroup->next;

	
	num_of_node--;
	return retGroup;
}
*/

void topoUnit::connectGroup(groupUnit* group1, groupUnit* group2){
	nodeUnit* node1 = group1->nodeList[rand() % group1->nodeList.size()];
	nodeUnit* node2 = group2->nodeList[rand() % group2->nodeList.size()];
	linkUnit newLink(node1, node2);

	linkList.push_back(newLink);
	for(int i = 0; i < group2->nodeList.size(); i++){
		group1->nodeList.push_back(group2->nodeList[i]);
	}
}

void topoUnit::output(char* fileName){
	FILE* output = fopen(fileName, "w");

	for(int i = 0; i < linkList.size(); i++){
		double newCost = (double) (rand() % 10) + 1.0;
		//newCost = 1.0;
		//fprintf(output, "%d\t%d\t%d\t%f\n", i, linkList[i].end0->nodeID, linkList[i].end1->nodeID, linkList[i].weight);
		fprintf(output, "%d\t%d\t%d\t%f\n", i, linkList[i].end0->nodeID, linkList[i].end1->nodeID, newCost);
	}

	fclose(output);
}

void topoUnit::setLink(int num_of_link){
	printf("num of link 1 = %d\n", linkList.size());
	if(num_of_link > linkList.size()){
		int numExtraLink = num_of_link - linkList.size();
		for(int i = 0; i < numExtraLink; i++){
			linkUnit newLink(NULL, NULL); // = NULL;
			do{
				//if(newLink) delete newLink;
				int index_end0 = rand() % groupList[0].nodeList.size();
				int index_end1 = rand() % groupList[0].nodeList.size();

				while(index_end0 == index_end1){
					index_end0 = rand() % groupList[0].nodeList.size();
					index_end1 = rand() % groupList[0].nodeList.size();
				}

				//newLink = new linkUnit(groupList[0].nodeList[index_end0]
				//	, groupList[0].nodeList[index_end1]);
				newLink.end0 = groupList[0].nodeList[index_end0];
				newLink.end1 = groupList[0].nodeList[index_end1];

			}while(searchDuplicate(&newLink));

			linkList.push_back(newLink);
		}
	}
	printf("num of link 2 = %d\n", linkList.size());
}

bool topoUnit::searchDuplicate(linkUnit* newLink){
	for(int i = 0; i < linkList.size(); i++){
		if(linkList[i].end0->nodeID == newLink->end0->nodeID 
			&& linkList[i].end1->nodeID == newLink->end1->nodeID)
				return true;
		if(linkList[i].end0->nodeID == newLink->end1->nodeID
			&& linkList[i].end1->nodeID == newLink->end0->nodeID)
				return true;
	}
	return false;
}

int main(int argc, char *argv[]){
	if(argc != 3){
		cout << "One parameter is needed" << endl;
		exit(1);
	}

	int num_node = atoi(argv[1]); // in run.sh = 600個node
	int seed = atoi(argv[2]); // in run.sh變數seed

	srand(seed);

	topoUnit topo(num_node); 
	topo.process();
	//topo.output("output.txt");
	printf("link num = %d\n", topo.linkList.size());
	//topo.setLink(num_node * 2);//讓topo的連通度比較高，避免點跟點會連不到,所以*2
	topo.output(argv[2]); // topo ＝> 指出哪些switch之間有link
}
