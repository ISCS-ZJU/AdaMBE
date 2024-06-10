/*
 * undirectedgraph.cpp
 *
 *  Created on: Sep 24, 2018
 *      Author: apurba
 */

#include "undirectedgraph.h"

undirectedgraph::undirectedgraph() :
		_n_(0), _m_(0) {
	// TODO Auto-generated constructor stub

}

void undirectedgraph::readInBiAdjList(const string &fname) {

	triangle_compute_time = 0;

	tflag = 0;

	std::ifstream instream(fname.c_str());

	numVL = 0;
	numVR = 0;

	string line;
	int v = 0;

	while (instream.good() && !instream.eof()) {
		std::getline(instream, line);
		stringstream strm(line);

		if (line.empty() || line[0] == '%') continue;
    int u;
		while(strm >> u){
			int new_u = u*10;
			int new_v = v*10+1;

			if(_AdjList_[new_u].size() == 0)
				numVL++;
			if(_AdjList_[new_v].size() == 0)
				numVR++;

			addEdge(new_u, new_v);
		}
		v++;	
	}
	_n_ = _AdjList_.size();
}


void undirectedgraph::readInEdgeList(const string &fname) {

	triangle_compute_time = 0;

	tflag = 0;

	std::ifstream instream(fname.c_str());

	if (instream.good() && !instream.eof()) {

		while (true) {

			if (!instream.good() || instream.eof()) {
				break;
			}

			string line;
			std::getline(instream, line);
			stringstream strm(line);

			if (!line.empty() && strm.good() && !strm.eof()) {

				int u, v;

				strm >> u >> v;

				assert(u != v);	
				assert(u >= 0);
				assert(v >= 0);

				addEdge(u, v);

			}
		}

		_n_ = _AdjList_.size();
	}
}

void undirectedgraph::readInBiEdgeList(const string &fname) {

	triangle_compute_time = 0;

	tflag = 0;

	std::ifstream instream(fname.c_str());

	numVL = 0;
	numVR = 0;

	string line;
	std::getline(instream, line);
	if (instream.good() && !instream.eof()) {

		while (true) {

			if (!instream.good() || instream.eof()) {
				break;
			}

			std::getline(instream, line);
			stringstream strm(line);

            if(line[0] == '%')continue;
			if (!line.empty() && strm.good() && !strm.eof()) {

				int u, v;

				strm >> u >> v;

				//assert(u != v);	//id of a vertex in the left side can be same as the id of a vertex in the right side
				//assert(u >= 0);
				//assert(v >= 0);
				
				int new_u = u*10;
				int new_v = v*10+1;

				if(_AdjList_[new_u].size() == 0)
					numVL++;
				if(_AdjList_[new_v].size() == 0)
					numVR++;

				addEdge(new_u, new_v);

			}
		}

		_n_ = _AdjList_.size();
	}
}

void undirectedgraph::readInEdgeList(const string &fname, int tcountflag) {

	triangle_compute_time = 0;

	tflag = tcountflag;

	std::ifstream instream(fname.c_str());

	if (instream.good() && !instream.eof()) {

		while (true) {

			if (!instream.good() || instream.eof()) {
				break;
			}

			string line;
			std::getline(instream, line);
			stringstream strm(line);

			if (!line.empty() && strm.good() && !strm.eof()) {

				int u, v;

				strm >> u >> v;

				assert(u != v);
				assert(u >= 0);
				assert(v >= 0);

				addEdge(u, v);

			}
		}

		_n_ = _AdjList_.size();
	}
}

void undirectedgraph::addEdge(int u, int v) {

	if (_AdjList_[u].size() == 0)
		_n_++;
	if (_AdjList_[v].size() == 0)
		_n_++;

	if ((_AdjList_[u].find(v) == _AdjList_[u].end())
			&& (_AdjList_[v].find(u) == _AdjList_[v].end())) {

		_m_++;

		_AdjList_[u].insert(v);
		_AdjList_[v].insert(u);

		if (tflag == 1) {
			auto t_start = std::chrono::high_resolution_clock::now();

			unordered_set<int> cneighborhood;

			utils::unordered_intersect(_AdjList_[u], _AdjList_[v],
					&cneighborhood);

			for (int w : cneighborhood) {
				tri[u] = tri[u] + 1;
				tri[v] = tri[v] + 1;
				tri[w] = tri[w] + 1;
			}

			auto t_end = std::chrono::high_resolution_clock::now();

			triangle_compute_time += std::chrono::duration<double, std::milli>(
					t_end - t_start).count();
		}

	}

}

long undirectedgraph::timeTriangleCountMillis() {
	return triangle_compute_time;
}

bool undirectedgraph::ContainsEdge(int u, int v) {
	if (_AdjList_[u].find(v) == _AdjList_[u].end())
		return false;
	return true;
}

bool undirectedgraph::ContainsVertex(int u) {

	return _AdjList_.count(u) > 0;

}

long undirectedgraph::getLeftSize(){
	return numVL;
}

long undirectedgraph::getRightSize(){
	return numVR;
}

void undirectedgraph::readInAdjList(string const &fname) {

	std::ifstream instream(fname.c_str());

	if (instream.good() && !instream.eof()) {
		int vcount = 0;
		int ecount = 0;
		while (true) {
			if (!instream.good() || instream.eof()) {
				break;
			}
			string line;
			std::getline(instream, line);
			//cout << line << "\n";
			stringstream strm(line);
			if (!line.empty() && strm.good() && !strm.eof()) {
				int u;
				strm >> u;
				vcount++;
				int v;
				unordered_set<int> S;
				_AdjList_[u] = S;
				while (strm.good() && !strm.eof()) {
					strm >> v;
					assert(u != v);
					ecount++;
					_AdjList_[u].insert(v);
				}
			}
		}
		_n_ = vcount;
		_m_ = ecount;
	}
}

void undirectedgraph::print() {
	for (map<int, unordered_set<int> >::iterator iter = _AdjList_.begin();
			iter != _AdjList_.end(); iter++) {
		int u = iter->first;
		cout << u << " :";
		for (int v : _AdjList_[u]) {
			cout << v << " ";
		}
		cout << "\n";
	}
}

unordered_set<int>& undirectedgraph::neighbor(int v) {
	return _AdjList_[v];
}

int undirectedgraph::degreeOf(int &v) {

	return _AdjList_[v].size();
}

int undirectedgraph::numV() {
	assert(_n_ > 0);
	return _n_;
}

int undirectedgraph::numE() {
	//assert(_m_ > 0);
	return _m_;
}

unordered_set<int> undirectedgraph::V() {
	unordered_set<int> rset;
	for (map<int, unordered_set<int> >::iterator iter = _AdjList_.begin();
			iter != _AdjList_.end(); iter++) {
		int u = iter->first;
		rset.insert(u);
	}
	return rset;
}

void undirectedgraph::triangleCount() {
	for (map<int, unordered_set<int> >::iterator iter = _AdjList_.begin();
			iter != _AdjList_.end(); iter++) {
		int u = iter->first;
		triangleCount(u);
	}
}

void undirectedgraph::triangleCount(int u) {
	for (int w : _AdjList_[u]) {
		for (int x : _AdjList_[u]) {
			if (_AdjList_[w].count(x) > 0) {
				int count = tri[u];
				tri[u] = count + 1;
			}
		}
	}
}

void undirectedgraph::degeneracy() {
	set<long> s;
	long shift = (long) 1e9;
	for (map<int, unordered_set<int> >::iterator iter = _AdjList_.begin();
			iter != _AdjList_.end(); iter++) {
		int u = iter->first;
		degen[u] = _AdjList_[u].size();
		s.insert(degen[u] * shift + u);
	}

	set<long>::iterator it;
	while (!s.empty()) {
		it = s.begin();
		long element = *it;
		int v = (int) (element % shift);
		for (int w : _AdjList_[v]) {
			if (degen[w] > degen[v]) {
				s.erase(degen[w] * shift + w);
				degen[w] = degen[w] - 1;
				s.insert(degen[w] * shift + w);
			}
		}
	}

}

int undirectedgraph::triangleCountAt(int u) {
	return tri[u];
}

int undirectedgraph::degeneracyOf(int u) {
	return degen[u];
}

undirectedgraph::~undirectedgraph() {
	// TODO Auto-generated destructor stub
}

