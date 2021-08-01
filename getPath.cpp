#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>

#include "getPath.h"

using namespace std;

//#define DATA_RATE 6
#define FIX_RATE 1

void nodeLink::assign(int id1, int id2, float _cost){
        ends[0] = id1;
        ends[1] = id2;
        cost = _cost;
}

getPath::getPath(char* fileName){
	FILE* input_file = fopen(fileName, "r");
	link_count = 0;
	done_count = 0;
	horizon_count = 0;







	//for(int i = 0; i < 170; i++) nodes[i] = NULL;

        if(!input_file) return;

	while(!feof(input_file)){
		int dd;
                int id1;
                int id2;
                float cost;
		fscanf(input_file, "%d %d %d %f\n", &dd, &id1, &id2, &cost);

		if(id1 > B_size) B_size = id1;
		if(id2 > B_size) B_size = id2;
	}

	B_size++;

	for(int i = 0; i < B_size; i++){
                node newNode;
                nodes.push_back(newNode);
                int newDone;
                done.push_back(newDone);
                int newHorizon;
                horizon.push_back(newHorizon);
        }

	fseek(input_file, 0, SEEK_SET);

        while(!feof(input_file)){
                int dd;
                int id1;
                int id2;
                float cost;
		nodeLink newLink;
                fscanf(input_file, "%d %d %d %f\n", &dd, &id1, &id2, &cost);
		newLink.assign(id1, id2, cost);

		//links.push_back(newLink);

		//if(!nodes[id1]){
			//nodes[id1] = new node();
			nodes[id1].cost = 0.0;
	       		nodes[id1].next = NULL;
        		nodes[id1].bytes = 0;
        		nodes[id1].id = id1;
		//}

		//if(!nodes[id1]) cout << "nulllllllll" << endl;

		nodes[id1].links.push_back(newLink);

		//if(!nodes[id2]){
                        //nodes[id2] = new node();
                        nodes[id2].cost = 0.0;
                        nodes[id2].next = NULL;
                        nodes[id2].bytes = 0;
                        nodes[id2].id = id2;
                //}
		
                nodes[id2].links.push_back(newLink);



		link_count++;
                //printf("%d %d %f\n", id1, id2, cost);
        }

        fclose(input_file);

	rate[0] = 2;
        rate[1] = 4;
        rate[2] = 6;

        rate_count[0] = 0;
        rate_count[1] = 0;
        rate_count[2] = 0;


	/*
        B = new int*[B_size];

        for(int i = 0; i < B_size; i++){
                B[i] = new int[B_size];
                for(int j = 0; j < B_size; j++){
                        B_file >> B[i][j];
                }
        }
	*/
}

getPath::getPath(){
}

getPath::~getPath(){
	/*
	for(int i = 0; i < 170; i++){
		if(nodes[i]) delete nodes[i];
	}

	for(int i = 0; i < B_size; i++){
                delete B[i];
        }
        delete B;
	*/
}

bool getPath::checkExist(int id){
	for(int i = 0; i < horizon_count; i++){
		if(horizon[i] == id) return true;
	}
	for(int i = 0; i < done_count; i++){
		//cout << "done = " << done[i] << " new = " << id << endl;
                if(done[i] == id) return true;
        }
	return false;
}

void getPath::cutTree(int** tree, int size){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < nodes[i].links.size(); j++){
			if(nodes[i].links[j].ends[0] == i){
				if(tree[i][nodes[i].links[j].ends[1]] <= 0){
					nodes[i].links.erase(nodes[i].links.begin() + j);	
					j--;
				}
                        }
			else if(nodes[i].links[j].ends[1] == i){
                                if(tree[i][nodes[i].links[j].ends[0]] <= 0){
                                        nodes[i].links.erase(nodes[i].links.begin() + j);
                                        j--;
                                }
                        }
		}
	}
}

float getPath::getCost(int id1, int id2){
	if(nodes[id1].links.size() < nodes[id2].links.size()){
		for(int i = 0; i < nodes[id1].links.size(); i++){
			if(nodes[id1].links[i].ends[0] == id2 || nodes[id1].links[i].ends[1] == id2)
				return nodes[id1].links[i].cost;
		}
	}

	else{
		for(int i = 0; i < nodes[id2].links.size(); i++){
                        if(nodes[id2].links[i].ends[0] == id1 || nodes[id2].links[i].ends[1] == id1)
                                return nodes[id2].links[i].cost;
                }
        }



	return -1.0;
}

void getPath::modCost(int id1, int id2, float cost){
	for(int i = 0; i < link_count; i++){
                if((links[i].ends[0] == id1 && links[i].ends[1] == id2)
                        || (links[i].ends[1] == id1 && links[i].ends[0] == id2)){
                               links[i].cost = cost;
                }
        }
}

void getPath::removeHorizon(int id){
	for(int i = 0; i < horizon_count; i++){
		if(horizon[i] == id){
			for(int j = i + 1; j < horizon_count; j++){
				horizon[j - 1] = horizon[j];
			}
			horizon_count--;
			return;
		}
	}
}

vector < pair < int, int > > getPath::spanningTree(){
	done[done_count++] = 0;
	vector < pair < int, int > > ret;

	while(done_count < nodes.size()){
		float minCost = -1.0;
		int minNode;
		int minLink;
		int minOtherEnd;

		for(int i = 0; i < done_count; i++){
			for(int j = 0; j < nodes[done[i]].links.size(); j++){
				if(minCost < 0 || minCost > nodes[done[i]].links[j].cost){
					int otherEnd = nodes[done[i]].links[j].ends[0] + nodes[done[i]].links[j].ends[1]
						- done[i];

					if(!checkExist(otherEnd)){
						minCost = nodes[done[i]].links[j].cost;
						minNode = done[i];
						minLink = j;
						minOtherEnd = otherEnd;
					}
				}
			}
		}

		pair < int, int > newLink;
		if(minNode < minOtherEnd){
			newLink.first = minNode;
			newLink.second = minOtherEnd;
		}
		else{
			newLink.first = minOtherEnd;
                        newLink.second = minNode;
		}
		ret.push_back(newLink);

		done[done_count++] = minOtherEnd;
	}

	return ret;
}

		

node* getPath::getNode(int ingress, int egress){
	done_count = 0;
	horizon_count = 0;

	/*
	for(int i = 0; i < nodes.size(); i++) nodes[i] = NULL;

	if(nodes[ingress]) delete nodes[ingress];

	cout << "path initial done" << endl;

	nodes[ingress] = new node();
	nodes[ingress]->cost = 0.0;
	nodes[ingress]->next = NULL;
	nodes[ingress]->bytes = 0;
	nodes[ingress]->id = ingress;

	*/

	nodes[ingress].next = NULL;
	nodes[ingress].cost = 0.0;
	nodes[ingress].bytes = 0;

	int processID = ingress;
	int count = 0;
	do{
		getDone(processID);
		float minCost = -1.0;
		for(int i = 0; i < horizon_count; i++){
			if(minCost == -1 || minCost > nodes[horizon[i]].cost){
				minCost = nodes[horizon[i]].cost;
				processID = horizon[i];
			}
			
		}
		if(processID == egress){
			return &nodes[processID];
		}


		count++;
		//if(count >= 8) return NULL;
	} while(horizon_count != 0);
	return NULL;
}

vector < node * >getPath::getNode(int ingress, vector < int > egress){
        done_count = 0;
        horizon_count = 0;

        vector < node * > ret;


        nodes[ingress].next = NULL;
        nodes[ingress].cost = 0.0;
        nodes[ingress].bytes = 0;

        int processID = ingress;
        int count = 0;

        for(int i = 0; i < egress.size(); i++){
                node * newNode;
                ret.push_back(newNode);
        }

        do{
                getDone(processID);
                float minCost = -1.0;
                for(int i = 0; i < horizon_count; i++){
                        if(minCost == -1 || minCost > nodes[horizon[i]].cost){
                                minCost = nodes[horizon[i]].cost;
                                processID = horizon[i];
                        }

                }
                for(int i = 0; i < egress.size(); i++){
                        if(processID == egress[i]){
                                ret[i] = &nodes[processID];
                                //ret.push_back(&nodes[processID]);
                                break;
                        }
                }

                count++;
                //if(count >= 8) return NULL;
	} while(horizon_count != 0);
        return ret;
}


vector < node * > getPath::getNode2(int ingress, vector < int > egress){
	done_count = 0;
        horizon_count = 0;

	vector < node * > ret;


        nodes[ingress].next = NULL;
        nodes[ingress].cost = 0.0;
        nodes[ingress].bytes = 0;

        int processID = ingress;
        int count = 0;

	for(int i = 0; i < egress.size(); i++){
		node * newNode = NULL;
		ret.push_back(newNode);
	}

        do{
                getDone(processID);
                float minCost = -1.0;
                for(int i = 0; i < horizon_count; i++){
                        if(minCost == -1 || minCost > nodes[horizon[i]].cost){
                                minCost = nodes[horizon[i]].cost;
                                processID = horizon[i];
                        }

                }
		for(int i = 0; i < egress.size(); i++){
                	if(processID == egress[i]){
				ret[i] = &nodes[processID];
                        	//ret.push_back(&nodes[processID]);
				//break;
				return ret;
                	}
		}

                count++;
                //if(count >= 8) return NULL;
         } while(horizon_count != 0);
	return ret;
}
	

void getPath::getDone(int id){
	removeHorizon(id);
	done[done_count++] = id;


	for(int i = 0; i < horizon_count; i++){
		int link_cost = getCost(id, horizon[i]);
		if(link_cost == -1.0) continue;
		if(nodes[horizon[i]].cost > nodes[id].cost + link_cost){
			
			nodes[horizon[i]].cost = nodes[id].cost + link_cost;
			nodes[horizon[i]].next = &nodes[id];
		}
	}

	for(int i = 0; i < nodes[id].links.size(); i++){
	//for(int i = 0; i < link_count; i++){
		int new_ID = -1;
		float cost = 0.0;


		if(nodes[id].links[i].ends[0] == id){

			new_ID = nodes[id].links[i].ends[1];
			cost = nodes[id].links[i].cost;


		}
		else if(nodes[id].links[i].ends[1] == id){
			new_ID = nodes[id].links[i].ends[0];
			cost = nodes[id].links[i].cost;
		}

		if(new_ID != -1 && !checkExist(new_ID)){


			horizon[horizon_count++] = new_ID;
			//nodes[new_ID] = new node();
			nodes[new_ID].cost = cost + nodes[id].cost;
			nodes[new_ID].next = &nodes[id];
			//nodes[new_ID]->id = new_ID;
		}
	}

}

/*
void getPath::schedule(){
	FILE* output_file = fopen("paths.txt", "w");
	getPath topo("output.txt");
	node* egressNode;
	node* temp;
	int max_id = 69;

	int num_of_path = 14;

	int count_yes = 0, count_no = 0;





	ifstream P_file("P.txt", ifstream::in);

        P_file >> P_size;

        P = new int*[P_size];

        for(int i = 0; i < P_size; i++){
                P[i] = new int[P_size];
                for(int j = 0; j < P_size; j++){
                        P_file >> P[i][j];
			bool fit = false;
			int data_rate;
			if(P[i][j] > 0){
				egressNode = topo.getNode(i, j);
                		temp = egressNode;
				int prev_id = -1;
				while(temp){
					if(prev_id >= 0){
						for(int k = 2; k >= 0; k--){
							data_rate = k;
							if(FIX_RATE) data_rate = 2;
							if(B[prev_id][temp->id] >= rate[data_rate]){
								fit = true;
								break;
							//B[prev_id][temp->id] -= DATA_RATE;
							}
						}
					}
						
                        		fprintf(output_file, "%d\t", temp->id);
					prev_id = temp->id;
                        		temp = temp->next;
                		}
				if(fit){
					fprintf(output_file, "(Allocated: %d)", rate[data_rate]);
					count_yes++;
					rate_count[data_rate]++;
				}
				else{
					fprintf(output_file, "(Not Allocated)");
					count_no++;
				}
                		fprintf(output_file, "\n");

				temp = egressNode;
				prev_id = -1;
				while(temp){
					if(prev_id >= 0){
						if(B[prev_id][temp->id] >= rate[data_rate]){
							B[prev_id][temp->id] -= rate[data_rate];
							B[temp->id][prev_id] -= rate[data_rate];
							if(!FIX_RATE) modCost(prev_id, temp->id, (float)(10 - B[prev_id][temp->id] + 1) * 10);
							if(!FIX_RATE) modCost(temp->id, prev_id, (float)(10 - B[temp->id][prev_id] + 1) * 10);
						}
					}
					prev_id = temp->id;
                                        temp = temp->next;
				}
			}
                }
        }


        P_file.close();


	fclose(output_file);

	for(int i = 0; i < P_size; i++){
		delete P[i];
	}
	delete P;


	printf("Allocated = %d, not allocated = %d\n", count_yes, count_no);
	printf("Rate %d: %d, Rate %d: %d, Rate %d: %d\n", rate[0], rate_count[0]
		, rate[1], rate_count[1], rate[2], rate_count[2]);
}

*/

/*
int main(){
	FILE* output_file = fopen("paths.txt", "w");
        getPath topo("output.txt");
        node* egressNode;
        node* temp;
        int max_id = 69;

        int num_of_path = 14;
}
*/
