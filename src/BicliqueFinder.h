#ifndef MBEA_BICLIQUE_FINDER_H
#define MBEA_BICLIQUE_FINDER_H

#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/parallel_for_each.h>
#include <tbb/global_control.h>

#include <atomic>
#include <bitset>
#include <cstring>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>

#include "BiGraph.h"
#ifndef NN
#define NN 64 
#endif
#define INTERVAL 1000000000
// #define COMPUTE_LEVEL

class BicliqueFinder {
 public:
  BicliqueFinder() = delete;
  BicliqueFinder(BiGraph *graph_in, const char *name);
  virtual void Execute(int min_l_size = 1, int min_r_size = 1) = 0;
  void PrintResult(char *fn = nullptr);
  void SetMemory(double memory_usage);
  void SetThreads(int threads);
  friend class MEBFinder;

 protected:
  BiGraph *graph_;
  Biclique maximum_biclique_;
  char *finder_name_;
  std::atomic<long long int> processing_nodes_, maximal_nodes_;
  int min_l_size_, min_r_size_;
  double exe_time_, start_time_;
  bool is_transposed_;
  double memory_usage_;
  int threads_;
  int cur_v_;
#ifdef COMPUTE_LEVEL
  int max_level_;
  long long int level_accumulation_;
  int cur_level_;
#endif
#ifdef PROFILE
  double bitset_time_;
#endif

  void setup(int min_l_size, int min_r_size);
  void finish();

  /**
   * @brief
   *
   * @param X stands for R
   * @param GamaX stands for L
   * @param tailX stands for C
   */
  void MineLMBC(std::vector<int> X, std::vector<int> GamaX,
                std::vector<int> tailX);
};

#endif
