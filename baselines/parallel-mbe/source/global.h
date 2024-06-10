/*
 * global.h
 *
 *  Created on: Oct 12, 2018
 *      Author: apurba
 */
#define TBB_PREVIEW_GLOBAL_CONTROL 1
#ifndef SOURCE_GLOBAL_H_
#define SOURCE_GLOBAL_H_

#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include "oneapi/tbb/concurrent_hash_map.h"
#include "oneapi/tbb/concurrent_unordered_set.h"
#include "oneapi/tbb/concurrent_vector.h"
#include "oneapi/tbb/blocked_range.h"
#include "oneapi/tbb/parallel_for.h"
#include "oneapi/tbb/parallel_for_each.h"
#include "oneapi/tbb/tick_count.h"
#include "oneapi/tbb/parallel_sort.h"
#include "oneapi/tbb/global_control.h"
#include "oneapi/tbb/task_group.h"


#include "undirectedgraph.h"

using namespace std;


template<typename K, typename V>
using ConcurrentMap = tbb::concurrent_hash_map<K,V>;

template<typename K, typename V>
using UMap = unordered_map<K,V>;

#endif /* SOURCE_GLOBAL_H_ */
