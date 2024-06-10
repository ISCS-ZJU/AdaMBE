#ifndef PARADAPTIVE_FINDER_H
#define PARADAPTIVE_FINDER_H

#include <map>
#include <unordered_map>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/parallel_for_each.h>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/task_group.h>


#include "BicliqueFinder.h"
typedef std::bitset<NN> bitset_t;

class ParAdaMBEFinder : public BicliqueFinder {
 public:
  ParAdaMBEFinder() = delete;
  ParAdaMBEFinder(BiGraph *graph_in, int thread_num = 1, const char *name = "ParAdaMBEFinder");
  void Execute(int min_l_size = 1, int min_r_size = 1);

 protected:
  void biclique_find(VertexSet L, VertexSet R,
                     std::vector<std::pair<int, VertexSet>> C, int depth);
  void biclique_find_serial(VertexSet &L, VertexSet &R,
                     std::vector<std::pair<int, VertexSet>> &C, int idx);
  void biclique_find(bitset_t L, VertexSet R,
                     std::vector<std::pair<int, bitset_t>> Q_C, int c_start);
  int AcquireFreeIdx(); 
  void FreeIdx(const int &idx);
  int thread_num;
  volatile std::atomic<unsigned long long> task_num;
  std::vector<std::vector<int>> idx_bufs;
  std::vector<std::vector<VertexSet>> global_2d_bufs;
  std::vector<std::mutex> shared_locks;
  tbb::task_group tg;
};

// /**
//  * @brief
//  * ParAdaMBEFinder is the parallel version of AdaMBEFinder
//  */
#endif
