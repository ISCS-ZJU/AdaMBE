/*
 * Lmbc.cpp
 *
 *  Created on: Jan 3, 2019
 *      Author: apurba
 */

#include "Lmbc.h"

Lmbc::Lmbc() {
	// TODO Auto-generated constructor stub
	ms = 1;
	maximal_biclique_count = 0;
}

Lmbc::Lmbc(undirectedgraph& g, int minsize){
	_g_ = g;
	ms = minsize;
	maximal_biclique_count = 0;
}

Lmbc::Lmbc(undirectedgraph& g){
	_g_ = g;
	ms = 1;
	maximal_biclique_count = 0;
}

void Lmbc::run(string outfile){
	unordered_set<int> X;
	unordered_set<int> gammaX;
	unordered_set<int> tailX;

	of = outfile;

	futile_computation_time = 0;
	sorting_time = 0;
	total_tail_elapsed = 0;

	//gammaX = _g_.V();
	//tailX = _g_.V();

	for (map<int, unordered_set<int> >::iterator iter = _g_._AdjList_.begin(); iter != _g_._AdjList_.end(); iter++){
	
		int u = iter->first;

		if(u % 10 == 0)
			tailX.insert(u);
		else
			gammaX.insert(u);
	
	}

	if(gammaX.size() > tailX.size()) 
		MineLMBC(X, gammaX, tailX, ms);
	else
		MineLMBC(X, tailX, gammaX, ms);

	cout << "Futile computation time: " << futile_computation_time << " ms.\n";
	cout << "sorting time: " << sorting_time << " ms.\n";
	cout << "iterating over tail vertices time: " << total_tail_elapsed << " ms.\n";
}

void Lmbc::MineLMBC(unordered_set<int>& X, unordered_set<int>& gammaX, unordered_set<int>& tailX, int ms){

	unordered_set<int>::iterator tail_iterator = tailX.begin();
	unordered_set<int>::iterator tail_end_iterator = tailX.end();

	vector<long> sorted_tailX;

	unordered_map<int, unordered_set<int>> M;

	auto tail_start = std::chrono::high_resolution_clock::now();
	
	while(tail_iterator != tail_end_iterator){

		int v = *tail_iterator;

		unordered_set<int> insect;

		utils::unordered_intersect(gammaX, _g_.neighbor(v), &insect);

		int size = insect.size();

		if(size < ms /*|| (v % 10 == 1)*/){
			tail_iterator = tailX.erase(tail_iterator);
		} else {
			long val = size*offset + v;

			M[v] = insect;

			sorted_tailX.push_back(val);
			tail_iterator++;
		}
	}
	
	auto tail_end = std::chrono::high_resolution_clock::now();

	auto tail_elapsed = std::chrono::duration<double, std::milli>(tail_end - tail_start).count();

	total_tail_elapsed += tail_elapsed;

	if(X.size() + tailX.size() < ms)
		return;

	auto sort_start = std::chrono::high_resolution_clock::now();
	
	std::sort(sorted_tailX.begin(), sorted_tailX.end());
	
	auto sort_end = std::chrono::high_resolution_clock::now();
			
	auto sort_elapsed = std::chrono::duration<double, std::milli>(sort_end - sort_start).count();

	sorting_time += sort_elapsed;

	for(int i = 0; i < sorted_tailX.size(); i++){

			
		int v = sorted_tailX[i] % offset;

		tailX.erase(v);

		if(X.size() + tailX.size() + 1 >= ms){

			unordered_set<int>::iterator gamma_x_union_v_iterator = M[v].begin();
			unordered_set<int>::iterator gamma_x_union_v_end_iterator = M[v].end();

			int vertex = *gamma_x_union_v_iterator;

			auto t_start = std::chrono::high_resolution_clock::now();

			unordered_set<int> Y = _g_.neighbor(vertex);

			gamma_x_union_v_iterator++;

			while(gamma_x_union_v_iterator != gamma_x_union_v_end_iterator){

				unordered_set<int> temp;
				int v = *gamma_x_union_v_iterator;
				utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

				Y = temp;
				gamma_x_union_v_iterator++;

				if(Y.size() == 0)
					break;
			}

			bool flag = true;

			for(auto w : Y){

				if((w != v) && X.count(w) == 0){
					if(tailX.count(w) == 0){
						flag = false;
						break;
					}
				}
			}
			
			auto t_end = std::chrono::high_resolution_clock::now();

			auto elapsed = std::chrono::duration<double, std::milli>(t_end - t_start).count();

			if(flag){
				if(Y.size() >= ms){

					maximal_biclique_count++;
					
					 if (maximal_biclique_count % 1000000 == 0){
                                                ofstream outfile;
                                                outfile.open(of.c_str(), ios::app);
                                                outfile << maximal_biclique_count << " maximal bicliques generated\n";
                                                outfile.close();
                                        }

				}
				unordered_set<int> newtailX;
				auto tail_start = std::chrono::high_resolution_clock::now();
				for(int y : tailX){
					if(Y.count(y) == 0)
						newtailX.insert(y);
				}
				
				auto tail_end = std::chrono::high_resolution_clock::now();

				auto tail_elapsed = std::chrono::duration<double, std::milli>(tail_end - tail_start).count();

				total_tail_elapsed += tail_elapsed;
				
				MineLMBC(Y, M[v], newtailX, ms);
			} else {
				futile_computation_time += elapsed;
			}
		}
	}
}

long Lmbc::getCount(){
	return maximal_biclique_count;
}

Lmbc::~Lmbc() {
	// TODO Auto-generated destructor stub
}

