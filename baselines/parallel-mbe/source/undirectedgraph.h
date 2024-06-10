/*
 * undirectedgraph.h
 *
 *  Created on: Sep 24, 2018
 *      Author: apurba
 */

#ifndef DS_UNDIRECTEDGRAPH_H_
#define DS_UNDIRECTEDGRAPH_H_

#include<map>
#include<set>
#include<iostream>
#include<string>
#include<sstream>
#include<cassert>
#include<iterator>
#include<fstream>
#include<cstdio>
#include<unordered_set>
#include<chrono>

#include "utils.h"

using namespace std;

class undirectedgraph {

	int _n_;	//number of vertices
	int _m_;	//number of edges
	map<int, int> tri;
	map<int, int> degen;
	void triangleCount(int);
	long triangle_compute_time;
	int tflag;
        long numVL;
	long numVR;
public:
	map<int, unordered_set<int> > _AdjList_;
	undirectedgraph();
	void readInAdjList(const string &);
  void readInBiAdjList(const string &);
  void readInEdgeList(const string &);
  void readInBiEdgeList(const string &);
	void readInEdgeList(const string &, int);
	void print();
	int numV();
	unordered_set<int> V();
	int numE();
	unordered_set<int>& neighbor(int);
	int degreeOf(int&);
	void addEdge(int, int);
	bool ContainsEdge(int, int);
	bool ContainsVertex(int);
	void triangleCount();
	void degeneracy();
	int triangleCountAt(int);
	int degeneracyOf(int);
	long timeTriangleCountMillis();
	long getLeftSize();
	long getRightSize();
	virtual ~undirectedgraph();
};

#endif /* DS_UNDIRECTEDGRAPH_H_ */
