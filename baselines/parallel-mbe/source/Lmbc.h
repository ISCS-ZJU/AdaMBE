/*
 * Lmbc.h
 *
 *  Created on: Jan 3, 2019
 *      Author: apurba
 */

#ifndef SOURCE_LMBC_H_
#define SOURCE_LMBC_H_

#include "global.h"

class Lmbc {
	undirectedgraph _g_;	//input graph
	int ms;					//minimum size threshold
	long offset = 10e9;
	long maximal_biclique_count;
	string of;
	long futile_computation_time;
	long sorting_time;
	long total_tail_elapsed;
	void MineLMBC(unordered_set<int>&, unordered_set<int>&, unordered_set<int>&, int);
public:
	Lmbc();
	Lmbc(undirectedgraph &);
	Lmbc(undirectedgraph &, int);
	void run(string);
	long getCount();
	virtual ~Lmbc();
};

#endif /* SOURCE_LMBC_H_ */
