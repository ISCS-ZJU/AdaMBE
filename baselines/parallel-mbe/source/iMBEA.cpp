/*
 * iMBEA.cpp
 *
 *  Created on: Feb 8, 2019
 *      Author: apurba
 */

#include "iMBEA.h"

iMBEA::iMBEA() {
	// TODO Auto-generated constructor stub
	maximal_biclique_count = 0;
}

iMBEA::iMBEA(undirectedgraph &g){
	_g_ = g;
	maximal_biclique_count = 0;
}

void iMBEA::run(){
	unordered_set<int> P;
	unordered_set<int> Q;
	unordered_set<int> L;
	unordered_set<int> R;

	for (map<int, unordered_set<int> >::iterator iter = _g_._AdjList_.begin(); iter != _g_._AdjList_.end(); iter++){
		int u = iter->first;

		if(u % 10 == 0){
			L.insert(u);
		} else {
			P.insert(u);
		}
	}

	enumerate(P, Q, L, R);
}

void iMBEA::enumerate(unordered_set<int>& P, unordered_set<int>& Q, unordered_set<int>& L, unordered_set<int>& R){

	unordered_set<int>::iterator p_iterator = P.begin();
	unordered_set<int>::iterator p_end_iterator = P.end();

	while(p_iterator != p_end_iterator){

		int x = *p_iterator;
		p_iterator++;

		unordered_set<int> Rprime = R;
		Rprime.insert(x);

		unordered_set<int> Lprime;
		unordered_set<int> Lbar;

		for(int l : L){
			if(_g_.ContainsEdge(l,x)){
				Lprime.insert(l);
			} else {
				Lbar.insert(l);
			}
		}

		unordered_set<int> C;
		unordered_set<int> Pprime;
		unordered_set<int> Qprime;
		bool isMaximal = true;

		//unordered_set<int> NLprime;

		//for(int l : Lprime){
		//	for(int r : _g_.neighbor(l)){
		//		NLprime.insert(r);
		//	}
		//}

		for(int v : Q){
			unordered_set<int> Nv;
			for(int u : Lprime){
				if(_g_.ContainsEdge(u,v)){
					Nv.insert(u);
				}
			}

			if(Nv.size() == Lprime.size()){
				isMaximal = false;
				break;
			} else if(Nv.size() > 0){
				Qprime.insert(v);
			}
		}

		if(isMaximal){
			for(int v : P){
				if(v != x){
					unordered_set<int> Nv;
					for(int u : Lprime){
						if(_g_.ContainsEdge(u,v)){
							Nv.insert(u);
						}
					}

					if(Nv.size() == Lprime.size()){
						Rprime.insert(v);
						unordered_set<int> S;
						for(int u : Lbar){
							if(_g_.ContainsEdge(u,v)){
								S.insert(u);
							}
						}

						if(S.size() == 0){
							C.insert(v);
						}
					} else if(Nv.size() > 0){
						Pprime.insert(v);
					}
				}
			}
			maximal_biclique_count++;

			if(Pprime.size() != 0){
				enumerate(Pprime, Qprime, Lprime, Rprime);
			}
		}

		Q.insert(x);
		P.erase(x);
		for(int c : C){
			Q.insert(c);
			P.erase(c);
		}


	}

}

long iMBEA::getCount(){
	return maximal_biclique_count;
}

iMBEA::~iMBEA() {
	// TODO Auto-generated destructor stub
}

