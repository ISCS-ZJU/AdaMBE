#ifndef ADAPTIVE_FINDER_H
#define ADAPTIVE_FINDER_H

#include <map>
#include <unordered_map>

#include "BicliqueFinder.h"
typedef std::bitset<NN> bitset_t;
class AdaMBEFinder_DSB : public BicliqueFinder {
 public:
  AdaMBEFinder_DSB() = delete;
  AdaMBEFinder_DSB(BiGraph *graph_in, const char *name = "AdaMBEFinder_DSB");
  void Execute(int min_l_size = 1, int min_r_size = 1);

 protected:
  void biclique_find(const VertexSet &L, const VertexSet &R,
                     std::vector<std::pair<int, int>> &P_Q,  // with LN size
                     int p_start);
  void biclique_find(bitset_t L_bs, std::vector<bitset_t> P_Q_bs, int p_start);

#ifdef PROFILE
  double set_intersection_time_;
  std::map<int, double> time_map_;                  // (|L|, time consumption)
  std::map<int, std::vector<double>> density_map_;  // (|L|, density list)
#endif
};

class AdaMBEFinder_NC : public BicliqueFinder {
 public:
  AdaMBEFinder_NC() = delete;
  AdaMBEFinder_NC(BiGraph *graph_in, const char *name = "AdaMBEFinder_NC");
  void Execute(int min_l_size = 1, int min_r_size = 1);

 protected:
  void biclique_find(VertexSet &L, VertexSet &R, std::vector<Node *> &C);
  double r_clock_;
  std::vector<VertexSet> global_2d_buf_;

#ifdef PROFILE
  unsigned long long int pruned_nodes_;
  unsigned long long real_vertices_;
  unsigned long long total_vertices_;
#endif
};

class AdaMBEFinder : public BicliqueFinder {
 public:
  AdaMBEFinder() = delete;
  AdaMBEFinder(BiGraph *graph_in, OrderEnum order = RInc, const char *name = "AdaMBEFinder");
  void Execute(int min_l_size = 1, int min_r_size = 1);

 protected:
  void biclique_find(VertexSet &L, VertexSet &R,
                     std::vector<std::pair<int, VertexSet>> &C);
  void biclique_find(bitset_t &L, VertexSet &R,
                     std::vector<std::pair<int, bitset_t>> &Q_C, int c_start);

  std::vector<VertexSet> global_2d_buf_;
  std::vector<int> idx_buf_;
  OrderEnum order_;
};

// /**
//  * @brief
//  * final version: AdaMBEFinder
//  * disable neighbor caching: AdaMBEFinder_DSB
//  * disable dynamic subgraph using bitmap: AdaMBEFinder_NC
//  */
#endif
