/*
 * ParLmbc.cpp
 *
 *  Created on: Jan 5, 2019
 *      Author: apurba
 */

#include "ParLmbc.h"
//#define TIME_TEST

ParLmbc::ParLmbc() {
	// TODO Auto-generated constructor stub

	ms = 1;
	maximal_biclique_count = 0;
}

ParLmbc::ParLmbc(undirectedgraph& g, int minsize){
	_g_ = g;
	ms = minsize;
	maximal_biclique_count = 0;
}

ParLmbc::ParLmbc(undirectedgraph& g){
	_g_ = g;
	ms = 1;
	maximal_biclique_count = 0;
}

void ParLmbc::run(string outfile){
	tbb::concurrent_unordered_set<int> X;
	tbb::concurrent_unordered_set<int> gammaX;
	tbb::concurrent_unordered_set<int> tailX;

	of = outfile;


	tbb::parallel_for_each(_g_._AdjList_.begin(), _g_._AdjList_.end(), [&](pair<int, unordered_set<int> > p){

		int v = p.first;

		if(v % 10 == 0)
			tailX.insert(v);
		else
			gammaX.insert(v);


	});

	if(gammaX.size() > tailX.size())
		ParMineLMBC(X, gammaX, tailX, ms);
	else
		ParMineLMBC(X, tailX, gammaX, ms);
}

void ParLmbc::runParMBE(string outfile, int nthreads){
        auto mp = tbb::global_control::max_allowed_parallelism;
        tbb::global_control gc(mp, nthreads);

	of = outfile;

	int bit = 0;

	if(_g_.getLeftSize() > _g_.getRightSize())
		bit = 1;
	else
		bit = 0;

	tbb::parallel_for_each(_g_._AdjList_.begin(), _g_._AdjList_.end(), [&](pair<int, unordered_set<int> > p){

		int v = p.first;

		if(v % 10 == bit){

			tbb::concurrent_unordered_set<int> X;
			tbb::concurrent_unordered_set<int> tailX;
			tbb::concurrent_unordered_set<int> gammaX;

			tailX.insert(v);

			//X.insert(v);

			tbb::parallel_for_each(p.second.begin(), p.second.end(), [&](int u){

				gammaX.insert(u);
			
				for(auto w : _g_.neighbor(u)){
					if(_g_.degreeOf(w) > _g_.degreeOf(v)){
						tailX.insert(w);
					}

					if(_g_.degreeOf(w) == _g_.degreeOf(v)){
						if(w > v)
							tailX.insert(w);
					}
				}

			});


			if(tailX.size() >= 1500)
				ParMineLMBC(X, gammaX, tailX, ms, v);
			else
				MineLMBC(X, gammaX, tailX, ms, v);
				
		}

	});
}

void ParLmbc::runParMBESeq(string outfile){


	of = outfile;

	int bit = 0;

  #ifdef TIME_TEST
  auto function_start = chrono::high_resolution_clock::now();
  #endif
	
  if(_g_.getLeftSize() > _g_.getRightSize())
		bit = 1;
	else
		bit = 0;

  #ifdef TIME_TEST
  auto start = chrono::high_resolution_clock::now();
  auto end = chrono::high_resolution_clock::now();
  #endif
  
  for(auto const& x : _g_._AdjList_){
    #ifdef TIME_TEST
    end = chrono::high_resolution_clock::now();
    loop_time += chrono::duration<double>(end - start).count();
    #endif
		int v = x.first;

    #ifdef TIME_TEST
    start = chrono::high_resolution_clock::now();
    #endif
		if(v % 10 == bit){

			unordered_set<int> X;
			unordered_set<int> tailX;
			unordered_set<int> gammaX;

			//tailX.insert(v);

			X.insert(v);

			for(auto u : x.second){

				gammaX.insert(u);
			
				for(auto w : _g_.neighbor(u)){
					if(_g_.degreeOf(w) > _g_.degreeOf(v)){
						tailX.insert(w);
					}

					if(_g_.degreeOf(w) == _g_.degreeOf(v)){
						if(w > v)
							tailX.insert(w);
					}
				}

			}
      #ifdef TIME_TEST
      end = chrono::high_resolution_clock::now();
      generate_set_time += chrono::duration<double>(end - start).count();

			//tailX.unsafe_erase(v);

      auto function_end = chrono::high_resolution_clock::now();
      function_time += chrono::duration<double>(function_end - function_start).count();
      #endif
			MineLMBCSeq(X, gammaX, tailX, ms, v);
      #ifdef TIME_TEST
      function_start = chrono::high_resolution_clock::now();
      #endif
			
		}
    #ifdef TIME_TEST
    start = chrono::high_resolution_clock::now();
    #endif
	}
  #ifdef TIME_TEST
  auto function_end = chrono::high_resolution_clock::now();
  function_time += chrono::duration<double>(function_end - function_start).count();
  cout << "generate_set_time: " << generate_set_time << endl;
  #endif
}

void ParLmbc::runCDFS(string outfile){


	of = outfile;

	  int bit = 0;

        if(_g_.getLeftSize() > _g_.getRightSize())
                bit = 1;
        else
                bit = 0;


	tbb::parallel_for_each(_g_._AdjList_.begin(), _g_._AdjList_.end(), [&](pair<int, unordered_set<int> > p){

		int v = p.first;

		if(v % 10 == bit){

			unordered_set<int> X;
			tbb::concurrent_unordered_set<int> tailX;
			tbb::concurrent_unordered_set<int> gammaX;

			//tailX.insert(v);
			X.insert(v);

			undirectedgraph h;

			for(auto u : p.second){

				h.addEdge(v, u);

				gammaX.insert(u);
			
				for(auto w : _g_.neighbor(u)){
					
					h.addEdge(w,u);
					
					if(_g_.degreeOf(w) > _g_.degreeOf(v)){
						tailX.insert(w);
					}

					if(_g_.degreeOf(w) == _g_.degreeOf(v)){
						if(w > v)
							tailX.insert(w);
					}
				}
			}

			CDFS(h, X, gammaX, tailX, ms, v);
		}

	});
}

void ParLmbc::ParMineLMBC(tbb::concurrent_unordered_set<int>& X, tbb::concurrent_unordered_set<int>& gammaX, tbb::concurrent_unordered_set<int>& tailX, int ms){


	tbb::concurrent_vector<long> sorted_tailX;

	ConcurrentMap<int, tbb::concurrent_unordered_set<int>> M;

	tbb::concurrent_unordered_set<int> removed_from_tailX;

	tbb::parallel_for_each(tailX.begin(), tailX.end(),
			[&](int v){

				tbb::concurrent_unordered_set<int> insect;

				utils::unordered_intersect(gammaX, _g_.neighbor(v), &insect);

				int size = insect.size();

				if(size < ms /*|| (v % 10 == 1)*/){
					//removed_from_tailX.insert(v);
				} else {
					long val = size*offset + v;

					ConcurrentMap<int, tbb::concurrent_unordered_set<int>>::accessor ac;

					M.insert(ac, v);

					ac->second = insect;

					sorted_tailX.push_back(val);
				}

	});


	tbb::parallel_sort(sorted_tailX);	//sort the vertices in tailX


	if(X.size() + tailX.size() < ms)
		return;


	tbb::parallel_for(tbb::blocked_range<int>(0, sorted_tailX.size()),
			[&](tbb::blocked_range<int> r){
		for(int idx = r.begin(); idx < r.end(); idx++){

			long x = sorted_tailX[idx];

			int v = x % offset;

			unordered_set<int> newtailX;

			for(int i=idx+1; i < sorted_tailX.size(); i++){

				long w = sorted_tailX[i];

				int u = w % offset;

				newtailX.insert(u);
			}

		if(X.size() + newtailX.size() + 1 >= ms){

			ConcurrentMap<int, tbb::concurrent_unordered_set<int>>::accessor ac;

			M.find(ac, v);

			int gamma_x_union_v_size = (ac->second).size();
			tbb::concurrent_unordered_set<int> Y;
			if(gamma_x_union_v_size >= 10){

				//cout << "at line " << __LINE__ << " size: " << gamma_x_union_v_size << "\n\n"; 
			
				ConcurrentMap<int, int> M_v_to_count;	
				
				tbb::parallel_for_each((ac->second).begin(), (ac->second).end(), [&](int x){
				
					for(auto y : _g_.neighbor(x)){
					
						ConcurrentMap<int, int>::accessor a;

						M_v_to_count.insert(a, y);
						a->second += 1;

						if(gamma_x_union_v_size == a->second)
							Y.insert(y);

					}
				

				});


				
			} else {

				tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_iterator = (ac->second).begin();
				tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_end_iterator = (ac->second).end();


				int vertex = *gamma_x_union_v_iterator;

				
				Y.insert(_g_.neighbor(vertex).begin(), _g_.neighbor(vertex).end());


				gamma_x_union_v_iterator++;


				while(gamma_x_union_v_iterator != gamma_x_union_v_end_iterator){

					tbb::concurrent_unordered_set<int> temp;
					int v = *gamma_x_union_v_iterator;
					utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

					Y = temp;
					gamma_x_union_v_iterator++;

					if(Y.size() == 0)
						break;
				}
			}



			bool flag = true;

			for(auto w : Y){

				if((w != v) && X.count(w) == 0){
					if(newtailX.count(w) == 0){
						flag = false;
						break;
					}
				}
			}

			if(flag){
				if(Y.size() >= ms){

					maximal_biclique_count++;

					 if (maximal_biclique_count % 1000000 == 0){
                                                //ofstream outfile;
                                               // outfile.open(of.c_str(), ios::app);
                                                //outfile << maximal_biclique_count << " maximal bicliques generated\n";
                                                cout << maximal_biclique_count << " maximal bicliques generated\n";
                                               //outfile.close();
                                        }

					/*cout << "maximal biclique found\n";
					cout << "{ ";
					for(auto y : Y){
						cout << y <<", ";
					}
					cout << " }, { ";
					for(auto y : gamma_x_union_v){
						cout << y << ", ";
					}
					cout << " }\n";
					*/



				}
				
				
				tbb::concurrent_unordered_set<int> updatedtailX;
				tbb::parallel_for_each(newtailX.begin(), newtailX.end(), [&](int x){
					
					if(Y.count(x) == 0)
						updatedtailX.insert(x);

				});
				
				if(newtailX.size() >= 1000)
					ParMineLMBC(Y, ac->second, updatedtailX, ms);
				else
					MineLMBC(Y, ac->second, updatedtailX, ms);
			}
		}
	}
	});

}

void ParLmbc::ParMineLMBC(tbb::concurrent_unordered_set<int>& X, tbb::concurrent_unordered_set<int>& gammaX, tbb::concurrent_unordered_set<int>& tailX, int ms, int pivotvertex){


	tbb::concurrent_vector<long> sorted_tailX;

	ConcurrentMap<int, tbb::concurrent_unordered_set<int>> M;

	tbb::concurrent_unordered_set<int> removed_from_tailX;

	/*if(X.size() == 1 && ms == 1){
		
		tbb::concurrent_unordered_set<int>::iterator gamma_x_iterator = gammaX.begin();

		int v = *gamma_x_iterator;

		unordered_set<int> Y = _g_.neighbor(v);
		
		for(int v : gammaX){
			
			unordered_set<int> temp;
                        utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

                        Y = temp;

                       	if(Y.size() == 0)
                        	break;

		}

		if(X.size() == Y.size()){
			maximal_biclique_count.fetch_and_increment();
		}
	}*/

	tbb::parallel_for_each(tailX.begin(), tailX.end(),
			[&](int v){	

				tbb::concurrent_unordered_set<int> insect;

				utils::unordered_intersect(gammaX, _g_.neighbor(v), &insect);

				int size = insect.size();

				if(size < ms /*|| (v % 10 == 1)*/){
					//removed_from_tailX.insert(v);
				} else {
					long val = size*offset + v;

					ConcurrentMap<int, tbb::concurrent_unordered_set<int>>::accessor ac;

					M.insert(ac, v);

					ac->second = insect;

					sorted_tailX.push_back(val);
				}

	});


	tbb::parallel_sort(sorted_tailX);	//sort the vertices in tailX


	if(X.size() + tailX.size() < ms)
		return;


	tbb::parallel_for(tbb::blocked_range<int>(0, sorted_tailX.size()),
			[&](tbb::blocked_range<int> r){
		for(int idx = r.begin(); idx < r.end(); idx++){

			long x = sorted_tailX[idx];

			int v = x % offset;

			unordered_set<int> newtailX;

			for(int i=idx+1; i < sorted_tailX.size(); i++){

				long w = sorted_tailX[i];

				int u = w % offset;

				newtailX.insert(u);
			}

		if(X.size() + newtailX.size() + 1 >= ms){

			ConcurrentMap<int, tbb::concurrent_unordered_set<int>>::accessor ac;

			M.find(ac, v);

			int gamma_x_union_v_size = (ac->second).size();
			tbb::concurrent_unordered_set<int> Y;
			
			if(gamma_x_union_v_size >= 5){

				//cout << "at line " << __LINE__ << " size: " << gamma_x_union_v_size << "\n\n"; 
			
				ConcurrentMap<int, int> M_v_to_count;	
				
				tbb::parallel_for_each((ac->second).begin(), (ac->second).end(), [&](int x){
				
					for(auto y : _g_.neighbor(x)){
					
						ConcurrentMap<int, int>::accessor a;

						M_v_to_count.insert(a, y);
						a->second += 1;

						if(gamma_x_union_v_size == a->second)
							Y.insert(y);

					}
				

				});


				
			} else {

				tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_iterator = (ac->second).begin();
				tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_end_iterator = (ac->second).end();


				int vertex = *gamma_x_union_v_iterator;


				Y.insert(_g_.neighbor(vertex).begin(), _g_.neighbor(vertex).end());


				gamma_x_union_v_iterator++;


				while(gamma_x_union_v_iterator != gamma_x_union_v_end_iterator){

					tbb::concurrent_unordered_set<int> temp;
					int v = *gamma_x_union_v_iterator;
					utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

					Y = temp;
					
					gamma_x_union_v_iterator++;

					if(Y.size() == 0)
						break;
				}
			}


			bool flag = true;

			for(auto w : Y){

				if((w != v) && X.count(w) == 0){
					if(newtailX.count(w) == 0){
						flag = false;
						break;
					}
				}
			}

			if(flag && Y.count(pivotvertex) > 0){
				if(Y.size() >= ms){

					maximal_biclique_count++;

					 if (maximal_biclique_count % 10000000 == 0){
                                                //ofstream outfile;
                                                //outfile.open(of.c_str(), ios::app);
                                                //outfile << maximal_biclique_count << " maximal bicliques generated\n";
                                                cout << maximal_biclique_count << " maximal bicliques generated\n";
                                                //outfile.close();
                                        }

					/*
					cout << "maximal biclique found\n";
					cout << "{ ";
					for(auto y : Y){
						cout << y <<", ";
					}
					cout << " }, { ";
					for(auto y : gamma_x_union_v){
						cout << y << ", ";
					}
					cout << " }\n";
					*/



				}
				
				tbb::concurrent_unordered_set<int> updatedtailX;
				tbb::parallel_for_each(newtailX.begin(), newtailX.end(), [&](int x){
					
					if(Y.count(x) == 0)
						updatedtailX.insert(x);

				});
				
				if(updatedtailX.size() >= 1000)
					ParMineLMBC(Y, ac->second, updatedtailX, ms, pivotvertex);
				else
					MineLMBC(Y, ac->second, updatedtailX, ms, pivotvertex);
			}
		}
	}
	});

}

void ParLmbc::CDFS(undirectedgraph& h, unordered_set<int>& X, tbb::concurrent_unordered_set<int>& gammaX, tbb::concurrent_unordered_set<int>& tailX, int ms, int pivotvertex){

 	tbb::concurrent_unordered_set<int>::iterator tail_iterator = tailX.begin();
        tbb::concurrent_unordered_set<int>::iterator tail_end_iterator = tailX.end();

        set<long> sorted_tailX;

        unordered_map<int, tbb::concurrent_unordered_set<int>> M;
	
	if(X.size() == 1){
		
		tbb::concurrent_unordered_set<int>::iterator gamma_x_iterator = gammaX.begin();

		int v = *gamma_x_iterator;

		unordered_set<int> Y = _g_.neighbor(v);
		
		for(int v : gammaX){
			
			unordered_set<int> temp;
                        utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

                        Y = temp;

                       	if(Y.size() == 0)
                        	break;

		}

		if(X.size() == Y.size()){
			maximal_biclique_count++;
		}
	}

        while(tail_iterator != tail_end_iterator){

                int v = *tail_iterator;

                tbb::concurrent_unordered_set<int> insect;

                utils::unordered_intersect(gammaX, h.neighbor(v), &insect);

                int size = insect.size();

                if(size < ms /*|| (v % 10 == 1)*/){
                        tail_iterator = tailX.unsafe_erase(tail_iterator);
                } else {
                        long val = size*offset + v;

                        M[v] = insect;

                        sorted_tailX.insert(val);       //automatic sorting of the vertices of tailX
                        tail_iterator++;
                }
        }


        if(X.size() + tailX.size() < ms)
                return;
	
	for(auto x : sorted_tailX){


                int v = x % offset;

                tailX.unsafe_erase(v);

                if(X.size() + tailX.size() + 1 >= ms){
                        
                        tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_iterator = M[v].begin();
                        tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_end_iterator = M[v].end();


                        int vertex = *gamma_x_union_v_iterator;

                        unordered_set<int> Y = h.neighbor(vertex);

                        gamma_x_union_v_iterator++;

                        while(gamma_x_union_v_iterator != gamma_x_union_v_end_iterator){

                                unordered_set<int> temp;
                                int v = *gamma_x_union_v_iterator;
                                utils::unordered_intersect(h.neighbor(v), Y, &temp);

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

			int temp_vertex;
			int degree = INT_MAX;

			for(auto y : Y){
				if(_g_.degreeOf(y) < degree){
					degree = _g_.degreeOf(y);
					temp_vertex = y;
				}
				if(_g_.degreeOf(y) == degree){
					if(y < temp_vertex)
						temp_vertex = y;
				}
			}
			
                       //if(flag && Y.count(pivotvertex) > 0){
                       if(flag && (pivotvertex == temp_vertex)){
                                if(Y.size() >= ms){

                                        maximal_biclique_count++;

                                         if (maximal_biclique_count % 10000000 == 0){
                                                //ofstream outfile;
                                                //outfile.open(of.c_str(), ios::app);
                                                //outfile << maximal_biclique_count << " maximal bicliques generated\n";
                                                cout << maximal_biclique_count << " maximal bicliques generated\n";
                                                //outfile.close();
                                        }

                                }
                                tbb::concurrent_unordered_set<int> newtailX;
                                for(int y : tailX){
                                        if(Y.count(y) == 0)
                                                newtailX.insert(y);
                                }

                                CDFS(h, Y, M[v], newtailX, ms, pivotvertex);
                        }
                }
        }




}


long ParLmbc::getCount(){
	return maximal_biclique_count;
}


ParLmbc::~ParLmbc() {
	// TODO Auto-generated destructor stub
}

void ParLmbc::printTime() {
  cout << "Sort time: " << sorted_time << endl;
  cout << "Infer time: " << infer_time << endl;
  cout << "function_time: "<< function_time << endl;
  cout << "collection_time: " << collection_time << endl;
  cout << "loop time: " << loop_time << endl;
}

void ParLmbc::MineLMBCSeq(unordered_set<int>& X, unordered_set<int>& gammaX, unordered_set<int>& tailX, int ms, int pivotvertex) {

  #ifdef TIME_TEST
  auto function_start = chrono::high_resolution_clock::now();
  auto start = chrono::high_resolution_clock::now();
  #endif
  unordered_set<int>::iterator tail_iterator = tailX.begin();
  unordered_set<int>::iterator tail_end_iterator = tailX.end();
  vector<long> sorted_tailX;
  unordered_map<int, unordered_set<int>> M;
  #ifdef TIME_TEST
  auto end = chrono::high_resolution_clock::now();
  #endif
	
	if(X.size() == 1 && ms == 1){
    //start = chrono::high_resolution_clock::now();

		unordered_set<int>::iterator gamma_x_iterator = gammaX.begin();
		int v = *gamma_x_iterator;
		unordered_set<int> Y = _g_.neighbor(v);
		for(int v : gammaX){
			unordered_set<int> temp;
      utils::unordered_intersect(_g_.neighbor(v), Y, &temp);
      Y = temp;
      if(Y.size() == 0)
        break;
		}

		if(X.size() == Y.size()){
			maximal_biclique_count++;
		}
    #ifdef TIME_TEST
    end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();
    #endif
	}

  #ifdef TIME_TEST
  start = chrono::high_resolution_clock::now();
  #endif
  while(tail_iterator != tail_end_iterator){
    int v = *tail_iterator;
    unordered_set<int> insect;
    utils::unordered_intersect(gammaX, _g_.neighbor(v), &insect);
    int size = insect.size();

    if(size < ms){
      tail_iterator = tailX.erase(tail_iterator);
    } else {
      long val = size*offset + v;
      M[v] = insect;
      sorted_tailX.push_back(val);
      tail_iterator++;
    }
  }


  #ifdef TIME_TEST
  end = chrono::high_resolution_clock::now();
  collection_time += chrono::duration<double>(end - start).count();
  start = chrono::high_resolution_clock::now();
  #endif
  if(X.size() + tailX.size() < ms) {
    #ifdef TIME_TEST
    auto function_end = chrono::high_resolution_clock::now();
    function_time += chrono::duration<double>(function_end - function_start).count();
    #endif
    return;
  }
  
	std::sort(sorted_tailX.begin(), sorted_tailX.end());
  #ifdef TIME_TEST
  end = chrono::high_resolution_clock::now();
  sorted_time += chrono::duration<double>(end - start).count();

  start = chrono::high_resolution_clock::now();
  #endif
  for(int i = 0; i < sorted_tailX.size(); i++){
    #ifdef TIME_TEST
    end = chrono::high_resolution_clock::now();
    loop_time += chrono::duration<double>(end - start).count();
    start = chrono::high_resolution_clock::now();
    
    #endif
    int v = sorted_tailX[i] % offset;
    tailX.erase(v);
    
    #ifdef TIME_TEST
    end = chrono::high_resolution_clock::now();
    generate_set_time += chrono::duration<double>(end - start).count();
    #endif
    
    if(X.size() + tailX.size() + 1 >= ms){
      
      
      #ifdef TIME_TEST
      start = chrono::high_resolution_clock::now();
      #endif
      unordered_set<int> Y;
      unordered_set<int>::iterator gamma_x_union_v_iterator = M[v].begin();
      unordered_set<int>::iterator gamma_x_union_v_end_iterator = M[v].end();
      int vertex = *gamma_x_union_v_iterator;
      Y = _g_.neighbor(vertex);
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
      #ifdef TIME_TEST
      end = chrono::high_resolution_clock::now();
      collection_time += chrono::duration<double>(end - start).count();
      start = chrono::high_resolution_clock::now();
      #endif
      bool flag = true;
      for(auto w : Y){
        if((w != v) && X.count(w) == 0){
          if(tailX.count(w) == 0){
            flag = false;
          break;
          }
        }
      }
      #ifdef TIME_TEST
      end = chrono::high_resolution_clock::now();
      infer_time += chrono::duration<double>(end - start).count();
      start = chrono::high_resolution_clock::now();
      #endif

      if(flag && (Y.count(pivotvertex) > 0)){
        if(Y.size() >= ms){
          maximal_biclique_count++;
          if (maximal_biclique_count % 1000000 == 0){
            //cout << maximal_biclique_count << " maximal bicliques generated\n";
          }
        }
        #ifdef TIME_TEST
        end = chrono::high_resolution_clock::now();
        infer_time += chrono::duration<double>(end - start).count();

        start = chrono::high_resolution_clock::now();
        #endif
        unordered_set<int> newtailX;
        for(int y : tailX){
          if(Y.count(y) == 0)
            newtailX.insert(y);
        }
        #ifdef TIME_TEST
        end = chrono::high_resolution_clock::now();
        generate_set_time += chrono::duration<double>(end - start).count();
        auto function_end = chrono::high_resolution_clock::now();
        function_time += chrono::duration<double>(function_end - function_start).count();
        #endif
        MineLMBCSeq(Y, M[v], newtailX, ms, pivotvertex);
        #ifdef TIME_TEST
        function_start = chrono::high_resolution_clock::now();
        #endif
      }
    }
    #ifdef TIME_TEST
    start = chrono::high_resolution_clock::now();
    #endif
  }
  #ifdef TIME_TEST
  auto function_end = chrono::high_resolution_clock::now();
  function_time += chrono::duration<double>(function_end - function_start).count();
  #endif
}


void ParLmbc::MineLMBC(tbb::concurrent_unordered_set<int>& X, tbb::concurrent_unordered_set<int>& gammaX, tbb::concurrent_unordered_set<int>& tailX, int ms, int pivotvertex){

        tbb::concurrent_unordered_set<int>::iterator tail_iterator = tailX.begin();
        tbb::concurrent_unordered_set<int>::iterator tail_end_iterator = tailX.end();

        vector<long> sorted_tailX;

        unordered_map<int, tbb::concurrent_unordered_set<int>> M;
	
	/*if(X.size() == 1 && ms == 1){
		
		tbb::concurrent_unordered_set<int>::iterator gamma_x_iterator = gammaX.begin();

		int v = *gamma_x_iterator;

		unordered_set<int> Y = _g_.neighbor(v);
		
		for(int v : gammaX){
			
			unordered_set<int> temp;
                        utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

                        Y = temp;

                       	if(Y.size() == 0)
                        	break;

		}

		if(X.size() == Y.size()){
			maximal_biclique_count.fetch_and_increment();
		}
	}*/

        while(tail_iterator != tail_end_iterator){

                int v = *tail_iterator;

                tbb::concurrent_unordered_set<int> insect;

                utils::unordered_intersect(gammaX, _g_.neighbor(v), &insect);

                int size = insect.size();

                if(size < ms /*|| (v % 10 == 1)*/){
                        tail_iterator = tailX.unsafe_erase(tail_iterator);
                } else {
                        long val = size*offset + v;

                        M[v] = insect;

                        sorted_tailX.push_back(val);
                        tail_iterator++;
                }
        }


        if(X.size() + tailX.size() < ms)
                return;
        
	//std::sort(sorted_tailX.begin(), sorted_tailX.end());
	tbb::parallel_sort(sorted_tailX);

        for(int i = 0; i < sorted_tailX.size(); i++){


                int v = sorted_tailX[i] % offset;

                tailX.unsafe_erase(v);

                if(X.size() + tailX.size() + 1 >= ms){

			tbb::concurrent_unordered_set<int> Y;
			

			tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_iterator = M[v].begin();
			tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_end_iterator = M[v].end();


			int vertex = *gamma_x_union_v_iterator;


			Y.insert(_g_.neighbor(vertex).begin(), _g_.neighbor(vertex).end());


			gamma_x_union_v_iterator++;


			while(gamma_x_union_v_iterator != gamma_x_union_v_end_iterator){

				tbb::concurrent_unordered_set<int> temp;
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
		
                        if(flag && (Y.count(pivotvertex) > 0)){
                                if(Y.size() >= ms){

                                        maximal_biclique_count++;

                                         if (maximal_biclique_count % 10000000 == 0){
                                                //ofstream outfile;
                                                //outfile.open(of.c_str(), ios::app);
                                                //outfile << maximal_biclique_count << " maximal bicliques generated\n";
                                                cout << maximal_biclique_count << " maximal bicliques generated\n";
                                                //outfile.close();
                                        }

                                }
				
                                tbb::concurrent_unordered_set<int> newtailX;
				tbb::parallel_for_each(tailX.begin(), tailX.end(), [&](int x){
					
					if(Y.count(x) == 0)
						newtailX.insert(x);

				});

                                MineLMBC(Y, M[v], newtailX, ms, pivotvertex);
                        }
                }
        }
}

void ParLmbc::MineLMBC(tbb::concurrent_unordered_set<int>& X, tbb::concurrent_unordered_set<int>& gammaX, tbb::concurrent_unordered_set<int>& tailX, int ms){

        tbb::concurrent_unordered_set<int>::iterator tail_iterator = tailX.begin();
        tbb::concurrent_unordered_set<int>::iterator tail_end_iterator = tailX.end();

        vector<long> sorted_tailX;

        //unordered_map<int, unordered_set<int>> M;
        unordered_map<int, tbb::concurrent_unordered_set<int>> M;

        while(tail_iterator != tail_end_iterator){

                int v = *tail_iterator;

                tbb::concurrent_unordered_set<int> insect;

                utils::unordered_intersect(gammaX, _g_.neighbor(v), &insect);

                int size = insect.size();

                if(size < ms /*|| (v % 10 == 1)*/){
                        tail_iterator = tailX.unsafe_erase(tail_iterator);
                } else {
                        long val = size*offset + v;

                        M[v] = insect;

                        sorted_tailX.push_back(val);
                        tail_iterator++;
                }
        }


        if(X.size() + tailX.size() < ms)
                return;
        
	//std::sort(sorted_tailX.begin(), sorted_tailX.end());
	tbb::parallel_sort(sorted_tailX);

        for(int i = 0; i < sorted_tailX.size(); i++){


                int v = sorted_tailX[i] % offset;

                tailX.unsafe_erase(v);

                if(X.size() + tailX.size() + 1 >= ms){

			int gamma_x_union_v_size = M[v].size();
			tbb::concurrent_unordered_set<int> Y;
			

			if(gamma_x_union_v_size >= 5){
				ConcurrentMap<int, int> M_v_to_count;	
				
				tbb::parallel_for_each(M[v].begin(), M[v].end(), [&](int x){
				
					for(auto y : _g_.neighbor(x)){
					
						ConcurrentMap<int, int>::accessor a;

						M_v_to_count.insert(a, y);
						a->second += 1;

						if(gamma_x_union_v_size == a->second)
							Y.insert(y);

					}
				

				});
			} else {

                        	tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_iterator = M[v].begin();
                        	tbb::concurrent_unordered_set<int>::iterator gamma_x_union_v_end_iterator = M[v].end();

                        	int vertex = *gamma_x_union_v_iterator;


				Y.insert(_g_.neighbor(vertex).begin(), _g_.neighbor(vertex).end());


                        	gamma_x_union_v_iterator++;

                        	while(gamma_x_union_v_iterator != gamma_x_union_v_end_iterator){

                        	        tbb::concurrent_unordered_set<int> temp;
                        	        int v = *gamma_x_union_v_iterator;
                        	        utils::unordered_intersect(_g_.neighbor(v), Y, &temp);

                        	        Y = temp;
                        	        gamma_x_union_v_iterator++;

                        	        if(Y.size() == 0)
                        	                break;
                        	}

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
		
                        if(flag){
                                if(Y.size() >= ms){

                                        maximal_biclique_count++;

                                         if (maximal_biclique_count % 1000000 == 0){
                                                //ofstream outfile;
                                                //outfile.open(of.c_str(), ios::app);
                                                //outfile << maximal_biclique_count << " maximal bicliques generated - using sequential algo\n";
                                                cout << maximal_biclique_count << " maximal bicliques generated - using sequential algo\n";
                                                //outfile.close();
                                        }

                                }
				
				tbb::concurrent_unordered_set<int> newtailX;
				tbb::parallel_for_each(tailX.begin(), tailX.end(), [&](int x){
					
					if(Y.count(x) == 0)
						newtailX.insert(x);

				});

                                MineLMBC(Y, M[v], newtailX, ms);
                        }
                }
        }
}
