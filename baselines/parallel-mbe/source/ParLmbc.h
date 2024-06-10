/*
 * ParLmbc.h
 *
 *  Created on: Jan 5, 2019
 *      Author: apurba
 */

#ifndef SOURCE_PARLMBC_H_
#define SOURCE_PARLMBC_H_

#include "global.h"

class ParLmbc {
	undirectedgraph _g_;
	int ms;
	long offset = 10e9;
	std::atomic<long> maximal_biclique_count;
  double sorted_time = 0;
  double generate_set_time = 0;
  double function_time = 0;
  double infer_time = 0;
  double collection_time = 0;
  double loop_time = 0;
  double alloc_time = 0;
	string of;
	void ParMineLMBC(oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, int);
	void ParMineLMBC_modified(oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, int);
	void ParMineLMBC(oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, int, int);
	void MineLMBC(oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, int);
	void MineLMBC(oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, int, int);
	void MineLMBCSeq(unordered_set<int>&, unordered_set<int>&, unordered_set<int>&, int, int);
	void CDFS(undirectedgraph &, unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, oneapi::tbb::concurrent_unordered_set<int>&, int, int);
	void compute_half_intersection(oneapi::tbb::concurrent_unordered_set<int>&, unordered_set<int> *);
public:
	ParLmbc();
	ParLmbc(undirectedgraph &);
	ParLmbc(undirectedgraph &, int);
	void run(string);
	void runParMBE(string, int);
	void runParMBESeq(string);
	void runCDFS(string);
	long getCount();
  void printTime();
	virtual ~ParLmbc();
};

#endif /* SOURCE_PARLMBC_H_ */
