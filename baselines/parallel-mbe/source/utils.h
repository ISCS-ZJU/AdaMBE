/*
 * utils.h
 *
 *  Created on: Sep 25, 2018
 *      Author: apurba
 */

#ifndef SOURCE_UTILS_H_
#define SOURCE_UTILS_H_

#include<unordered_set>
#include<chrono>
using namespace std;

#include <oneapi/tbb/concurrent_unordered_set.h>

class utils {

	static int size;
  static double collection_time;
public:
  //static chrono::duration<double> collection_time;
  
	static void unordered_intersect(const unordered_set<int> &A,
			const unordered_set<int> &B, unordered_set<int> *result) {

    auto start = chrono::high_resolution_clock::now();
		if (A.size() > B.size()) {
			for (int i : B) {
				if (A.find(i) != A.end()){

					(*result).insert(i);
				}
			}
		} else {
			for (int i : A) {
				if (B.find(i) != B.end()){
					(*result).insert(i);
				}
			}
		}
    auto end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();

		//return *result;
	}
	
	static void unordered_intersect(const unordered_set<int> &A,
			const tbb::concurrent_unordered_set<int> &B, unordered_set<int> *result) {

    auto start = chrono::high_resolution_clock::now();
		if (A.size() > B.size()) {
			for (int i : B) {
				if (A.find(i) != A.end()){

					(*result).insert(i);
				}
			}
		} else {
			for (int i : A) {
				if (B.find(i) != B.end()){
					(*result).insert(i);
				}
			}
		}
    auto end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();

		//return *result;
	}
	
	static void unordered_intersect(const unordered_set<int> &A,
			const tbb::concurrent_unordered_set<int> &B, tbb::concurrent_unordered_set<int> *result) {

    auto start = chrono::high_resolution_clock::now();

		if (A.size() > B.size()) {
			for (int i : B) {
				if (A.find(i) != A.end()){

					(*result).insert(i);
				}
			}
		} else {
			for (int i : A) {
				if (B.find(i) != B.end()){
					(*result).insert(i);
				}
			}
		}

    auto end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();
		//return *result;
	}
	
	static void unordered_intersect(tbb::concurrent_unordered_set<int> &A,
			const unordered_set<int> &B, unordered_set<int> *result) {

    auto start = chrono::high_resolution_clock::now();
		if (A.size() > B.size()) {
			for (int i : B) {
				if (A.find(i) != A.end()){

					(*result).insert(i);
				}
			}
		} else {
			for (int i : A) {
				if (B.find(i) != B.end()){
					(*result).insert(i);
				}
			}
		}

    auto end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();
		//return *result;
	}
	
	static void unordered_intersect(tbb::concurrent_unordered_set<int> &A,
			const unordered_set<int> &B, tbb::concurrent_unordered_set<int> *result) {

    auto start = chrono::high_resolution_clock::now();
		if (A.size() > B.size()) {
			for (int i : B) {
				if (A.find(i) != A.end()){

					(*result).insert(i);
				}
			}
		} else {
			for (int i : A) {
				if (B.find(i) != B.end()){
					(*result).insert(i);
				}
			}
		}

    auto end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();
		//return *result;
	}

	static int intersection_size(){
		return size;
	}

	static int unordered_intersect_size(const unordered_set<int> &A,
			const unordered_set<int> &B) {

    auto start = chrono::high_resolution_clock::now();
		int count=0;

		if (A.size() > B.size()) {
			for (int i : B) {
				if (A.find(i) != A.end())
					count++;
			}
		} else {
			for (int i : A) {
				if (B.find(i) != B.end())
					count++;
			}
		}

    auto end = chrono::high_resolution_clock::now();
    collection_time += chrono::duration<double>(end - start).count();
		return count;
	}

  static double get_collection_time() {
    return collection_time;
  }
};

#endif /* SOURCE_UTILS_H_ */
