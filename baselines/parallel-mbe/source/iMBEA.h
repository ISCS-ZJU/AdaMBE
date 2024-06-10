/*
 * iMBEA.h
 *
 *  Created on: Feb 8, 2019
 *      Author: apurba
 */

#ifndef SOURCE_IMBEA_H_
#define SOURCE_IMBEA_H_

#include "global.h"

class iMBEA {
	undirectedgraph _g_;
	long offset = 10e9;
	long maximal_biclique_count;
	void enumerate(unordered_set<int>&, unordered_set<int>&, unordered_set<int>&, unordered_set<int>&);
public:
	iMBEA();
	iMBEA(undirectedgraph &);
	void run();
	long getCount();
	virtual ~iMBEA();
};

#endif /* SOURCE_IMBEA_H_ */
