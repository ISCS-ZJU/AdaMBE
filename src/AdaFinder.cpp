#include "AdaFinder.h"

AdaMBEFinder_DSB::AdaMBEFinder_DSB(BiGraph *graph_in, const char *name)
    : BicliqueFinder(graph_in, name) {}

void AdaMBEFinder_DSB::Execute(int min_l_size, int min_r_size) {
  setup(min_l_size, min_r_size);
  graph_->Reorder(RInc);
  for (cur_v_ = 0; cur_v_ < graph_->GetRSize(); cur_v_++) {
    if (graph_->NeighborsR(cur_v_).size() < min_l_size_) continue;
    if (cur_v_ > 0 && graph_->NeighborsR(cur_v_ - 1) == graph_->NeighborsR(cur_v_)) continue;
    processing_nodes_++;

#ifdef PROFILE
    double node_start_time, node_end_time, recur_start_time, recur_end_time,
        recur_time = 0;
    int valid_vertices = 0, total_vertices = 0;
    node_start_time = get_cur_time();
#endif

    VertexSet L, R;
    std::vector<std::pair<int, int>> P_Q;
    int p_start = graph_->GetRSize();

    L = graph_->NeighborsR(cur_v_);

    if (L.size() > NN) {
#ifdef PROFILE
      double set_start_time = get_cur_time();
#endif
      std::map<int, int> c_map;
      for (int w : L) {
        for (int y : graph_->NeighborsL(w)) {
          c_map[y]++;
        }
      }
#ifdef PROFILE
      set_intersection_time_ += get_cur_time() - set_start_time;
#endif
      for (auto c_node : c_map) {
#ifdef PROFILE
        if (L.size() != c_node.second) {
          total_vertices += L.size();
          valid_vertices += c_node.second;
        }
#endif
        if (c_node.second == L.size()) {
          R.emplace_back(c_node.first);
        } else if (c_node.second >= min_l_size_) {
          if (c_node.first > cur_v_ && p_start > P_Q.size()) p_start = P_Q.size();
          P_Q.emplace_back(c_node);
        }
      }

      if (R.size() >= min_r_size_) {
        if ((++maximal_nodes_) % INTERVAL == 0) {
          printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
        };
        maximum_biclique_.CompareAndSet(L, R, graph_);
      }
#ifdef PROFILE
      recur_start_time = get_cur_time();
#endif
      biclique_find(L, R, P_Q, p_start);
#ifdef PROFILE
      recur_end_time = get_cur_time();
      recur_time += recur_end_time - recur_start_time;
#endif
    } else {
#ifdef PROFILE
      double start = get_cur_time();
#endif
#if NN <= 64
      bitset_t L_bs((~0LLU) >> (64 - L.size()));
#else
      bitset_t L_bs;
      for (int i = 0; i < L.size(); i++) L_bs.set(i);
#endif
#ifdef PROFILE
      double set_start_time = get_cur_time();
#endif
      std::map<int, bitset_t> c_bs_map;
      for (int i = 0; i < L.size(); i++) {
        int l = L[i];
        for (int w : graph_->NeighborsL(l)) {
          c_bs_map[w].set(i);
        }
      }
#ifdef PROFILE
      set_intersection_time_ += get_cur_time() - set_start_time;
#endif

      std::vector<bitset_t> P_Q_bs;
      int p_start;
      for (auto &p : c_bs_map) {
#ifdef PROFILE
        total_vertices += L.size();
        valid_vertices += p.second.count();
#endif
        if (p.first == cur_v_)
          p_start = P_Q_bs.size();
        else if (p.second != L_bs)
          P_Q_bs.emplace_back(p.second);
      }
      if ((++maximal_nodes_) % INTERVAL == 0) {
        printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
               get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
      };
#ifdef PROFILE
      recur_start_time = get_cur_time();
#endif
      biclique_find(L_bs, P_Q_bs, p_start);
#ifdef PROFILE
      recur_end_time = get_cur_time();
      recur_time += recur_end_time - recur_start_time;
#endif
#ifdef PROFILE
      bitset_time_ += get_cur_time() - start;
#endif
    }
#ifdef PROFILE
    node_end_time = get_cur_time();
    time_map_[L.size()] += node_end_time - node_start_time - recur_time;
    if (total_vertices != 0)
      density_map_[L.size()].emplace_back(1.0 * valid_vertices /
                                          total_vertices);
#endif
  }

#ifdef PROFILE
  // printf("Time profile: (|L|, time)\n");
  // double time_all = 0;
  // for (auto p : time_map_) {
  //   printf("%d, %lf\n", p.first, p.second);
  //   time_all += p.second;
  // }
  // printf("\ntime all:%lf\n", time_all);
  printf("time set:%lf\n", set_intersection_time_);
  // printf("\n\nDensity profile: (|L|, density)\n");
  // for (auto p : density_map_) {
  //   double sum = 0;
  //   for (auto density : p.second) sum += density;
  //   if (p.second.size() != 0)
  //     printf("%d, %ld, %lf\n", p.first, p.second.size(), sum /
  //     p.second.size());
  // }
  // printf("\n\n");
#endif

  finish();
}

void AdaMBEFinder_DSB::biclique_find(const VertexSet &L, const VertexSet &R,
                                     std::vector<std::pair<int, int>> &P_Q,
                                     int p_start) {
#ifdef EXIT_TIME
  if (get_cur_time() - start_time_ >= EXIT_TIME) {
    finish();
    PrintResult();
    exit(0);
  }
#endif
  std::vector<int> LN(P_Q.size(), 0);
  std::vector<bitset_t> LN_bs(P_Q.size(), 0);

#ifdef PROFILE
  double node_start_time, node_end_time, recur_start_time, recur_end_time,
      recur_time = 0;
  int valid_vertices = 0, total_vertices = 0;
  node_start_time = get_cur_time();
#endif

  for (int pid = p_start; pid < P_Q.size(); pid++) {
    int v = P_Q[pid].first;
    if (v < 0) continue;
    processing_nodes_++;
    VertexSet L_prime = seq_intersect(L, graph_->NeighborsR(v));
    VertexSet R_prime;
    int p_start_prime = P_Q.size();
#ifdef PROFILE
    valid_vertices += L_prime.size();
    total_vertices += L.size();
#endif

    if (L_prime.size() > NN) {
      std::vector<std::pair<int, int>> P_Q_prime;
#ifdef PROFILE
      double set_start_time = get_cur_time();
#endif
      for (int l : L_prime) {  // compute local neighbors
        auto &l_neighbors = graph_->NeighborsL(l);
        for (int i = 0, j = 0; i < P_Q.size() && j < l_neighbors.size();) {
          if (P_Q[i].first < l_neighbors[j])
            i++;
          else if (P_Q[i].first > l_neighbors[j])
            j++;
          else {
            LN[i++]++;
            j++;
          }
        }
      }
#ifdef PROFILE
      set_intersection_time_ += get_cur_time() - set_start_time;
#endif
      bool is_maximal = true;
      for (int i = 0; i < pid && is_maximal; i++) {  // maximality check
        if (LN[i] == L_prime.size()) is_maximal = false;
      }

      if (is_maximal) R_prime = R;
      for (int i = 0; i < P_Q.size(); i++) {
        if (is_maximal) {
          if (LN[i] == L_prime.size())
            R_prime.emplace_back(P_Q[i].first);
          else if (LN[i] >= min_l_size_ && P_Q[i].first >= 0) {
            if (i > pid && p_start_prime > P_Q_prime.size())
              p_start_prime = P_Q_prime.size();
            P_Q_prime.emplace_back(std::make_pair(P_Q[i].first, LN[i]));
          }
        }
        if (i != pid && LN[i] == P_Q[i].second) P_Q[i].first = -1;
        LN[i] = 0;
      }

      if (is_maximal) {
        if (R_prime.size() >= min_r_size_) {
          if ((++maximal_nodes_) % INTERVAL == 0) {
            printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                   get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
          };
          maximum_biclique_.CompareAndSet(L_prime, R_prime, graph_);
        }
#ifdef PROFILE
        recur_start_time = get_cur_time();
#endif
        biclique_find(L_prime, R_prime, P_Q_prime, p_start_prime);
#ifdef PROFILE
        recur_end_time = get_cur_time();
        recur_time += recur_end_time - recur_start_time;
#endif
      }
    } else if (L_prime.size() >= min_l_size_) {
#ifdef PROFILE
      double start = get_cur_time();
#endif
#if NN <= 64
      bitset_t L_bs((~0LLU) >> (64 - L_prime.size()));
#else
      bitset_t L_bs;
      for (int i = 0; i < L_prime.size(); i++) L_bs.set(i);
#endif
#ifdef PROFILE
      double set_start_time = get_cur_time();
#endif
      for (int id = 0; id < L_prime.size(); id++) {  // compute local neighbors
        int l = L_prime[id];
        auto &l_neighbors = graph_->NeighborsL(l);
        for (int i = 0, j = 0; i < P_Q.size() && j < l_neighbors.size();) {
          if (P_Q[i].first < l_neighbors[j])
            i++;
          else if (P_Q[i].first > l_neighbors[j])
            j++;
          else {
            LN_bs[i++].set(id);
            j++;
          }
        }
      }

#ifdef PROFILE
      set_intersection_time_ += get_cur_time() - set_start_time;
#endif
      bool is_maximal = true;
      for (int i = 0; i < pid && is_maximal; i++) {  // maximality check
        if (LN_bs[i] == L_bs) is_maximal = false;
      }

      if (is_maximal) {
        R_prime = R;
        std::vector<bitset_t> P_Q_bs;
        for (int i = 0; i < P_Q.size(); i++) {
          if (LN_bs[i] == L_bs)
            R_prime.emplace_back(P_Q[i].first);
          else if (LN_bs[i].count() >= min_l_size_) {
            if (i > pid && p_start_prime > P_Q_bs.size())
              p_start_prime = P_Q_bs.size();
            P_Q_bs.emplace_back(LN_bs[i]);
          }
        }
        if (R_prime.size() >= min_r_size_) {
          if ((++maximal_nodes_) % INTERVAL == 0) {
            printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                   get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
          };
          maximum_biclique_.CompareAndSet(L_prime, R_prime, graph_);
        }
#ifdef PROFILE
        recur_start_time = get_cur_time();
#endif
        biclique_find(L_bs, P_Q_bs, p_start_prime);
#ifdef PROFILE
        recur_end_time = get_cur_time();
        recur_time += recur_end_time - recur_start_time;
#endif
      }

      for (int i = 0; i < P_Q.size(); i++) {
        if (i != pid && LN_bs[i].count() == P_Q[i].second)  // prune technique
          P_Q[i].first = -1;
        LN_bs[i].reset();
      }
#ifdef PROFILE
      bitset_time_ += get_cur_time() - start;
#endif
    }
  }

#ifdef PROFILE
  node_end_time = get_cur_time();
  time_map_[L.size()] += node_end_time - node_start_time - recur_time;
  if (total_vertices != 0)
    density_map_[L.size()].emplace_back(1.0 * valid_vertices / total_vertices);
#endif
}

void AdaMBEFinder_DSB::biclique_find(bitset_t L_bs,
                                     std::vector<bitset_t> P_Q_bs,
                                     int p_start) {
#ifdef EXIT_TIME
  if (get_cur_time() - start_time_ >= EXIT_TIME) {
    finish();
    PrintResult();
    exit(0);
  }
#endif
  for (int pid = p_start; pid < P_Q_bs.size(); pid++) {
    bitset_t cur_L_bs = L_bs & P_Q_bs[pid];
    if (cur_L_bs == 0) continue;

    bool is_maximal = true;
    int p_start_prime = P_Q_bs.size();
    std::vector<bitset_t> P_Q_bs_prime;
    processing_nodes_++;

#ifdef PROFILE
    double set_start_time = get_cur_time();
#endif
    for (int i = 0; i < P_Q_bs.size() && is_maximal; i++) {
      if ((P_Q_bs[i] & cur_L_bs) == cur_L_bs) {
        if (i < pid) is_maximal = false;
      } else if ((P_Q_bs[i] & cur_L_bs) != 0) {
        if (i > pid && p_start_prime > P_Q_bs_prime.size())
          p_start_prime = P_Q_bs_prime.size();
        P_Q_bs_prime.emplace_back(P_Q_bs[i] & cur_L_bs);
      }
    }
#ifdef PROFILE
    set_intersection_time_ += get_cur_time() - set_start_time;
#endif

    if (is_maximal) {
      biclique_find(cur_L_bs, P_Q_bs_prime, p_start_prime);
      if ((++maximal_nodes_) % INTERVAL == 0) {
        printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
               get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
      };
    }
    for (int i = 0; i < P_Q_bs.size(); i++) {  // prune technique
      if ((P_Q_bs[i] & cur_L_bs) == P_Q_bs[i] && i != pid) P_Q_bs[i] = 0;
    }
  }
}

AdaMBEFinder_NC::AdaMBEFinder_NC(BiGraph *graph_in, const char *name)
    : BicliqueFinder(graph_in, name) {}

void AdaMBEFinder_NC::Execute(int min_l_size, int min_r_size) {
#ifdef PROFILE
  pruned_nodes_ = 0;
  real_vertices_ = 0;
  total_vertices_ = 0;
#endif
  setup(min_l_size, min_r_size);

  graph_ = new BiGraph(*graph_);
  graph_->Reorder(RInc);

  global_2d_buf_ = std::vector<VertexSet>(graph_->GetRSize(), VertexSet());

  for (cur_v_ = 0; cur_v_ < graph_->GetRSize(); cur_v_++) {
    if (graph_->NeighborsR(cur_v_).size() < min_l_size_) continue;
    if (cur_v_ > 0 && graph_->NeighborsR(cur_v_ - 1) == graph_->NeighborsR(cur_v_)) continue;
    processing_nodes_++;

    VertexSet &L = graph_->NeighborsR(cur_v_);
    VertexSet R;
    std::vector<Node *> C;
    std::map<int, std::vector<int>> c_map;

    for (int l : L) {
      auto &l_neighbors = graph_->NeighborsL(l);
      for (int i = l_neighbors.size() - 1; l_neighbors[i] > cur_v_; i--) {
        c_map[l_neighbors[i]].emplace_back(l);
      }
    }
    R.emplace_back(cur_v_);
    for (auto &c_node : c_map) {
      if (c_node.second.size() == L.size())
        R.emplace_back(c_node.first);
      else if (c_node.second.size() >= min_l_size) {
        C.emplace_back(new Node(c_node.first, std::move(c_node.second)));
      }
    }

    if (R.size() >= min_r_size_) {
      if ((++maximal_nodes_) % INTERVAL == 0) {
        printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
               get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
      };
      maximum_biclique_.CompareAndSet(L, R, graph_);
    }

    biclique_find(L, R, C);

    for (auto ptr : C) delete ptr;
  }

  finish();

#ifdef PROFILE
  printf("Pruned nodes:%llu. Pruned vertex ratio:%lf.\n", pruned_nodes_,
         1 - 1.0 * real_vertices_ / total_vertices_);
#endif
}

void AdaMBEFinder_NC::biclique_find(VertexSet &L, VertexSet &R,
                                    std::vector<Node *> &C) {
#ifdef EXIT_TIME
  if (get_cur_time() - start_time_ >= EXIT_TIME) {
    finish();
    PrintResult();
    exit(0);
  }
#endif
  // std::vector<VertexSet> global_2d_buf_(C.size(), std::vector<int>());
  std::vector<Node *> create_node_ptrs;
  VertexSet Q;
#ifdef PROFILE
  for (int i = 0; i < C.size(); i++) {
    total_vertices_ += graph_->NeighborsR(C[i]->vc).size();
    real_vertices_ += C[i]->L.size();
  }
#endif
  // suppose that C[i].L is OK.
  for (int i = 0; i < C.size(); i++) {
#ifdef PROFILE
    if (!C[i]->R.empty()) pruned_nodes_++;
#endif
    if (!C[i]->R.empty() || C[i]->L.size() < min_l_size_) continue;
    processing_nodes_++;

    int last_l = -1;
    for (int j = 0; j < C[i]->L.size(); j++) {  // compute
      int l = C[i]->L[j];
      auto &l_neighbors = graph_->NeighborsL(l);
      int li = 0;

      if (j == 0) {  // generate vertex set Q
        int ri = 0;
        while (l_neighbors[li] < C[i]->vc) {
          if (ri >= R.size() || l_neighbors[li] < R[ri]) {
            Q.emplace_back(l_neighbors[li]);
            li++;
          } else {
            li++;
            ri++;
          }
        }
      } else if (!Q.empty()) {  // maximality check
        int q_valid = 0;
        for (int qi = 0; li < l_neighbors.size() && qi < Q.size();) {
          if (l_neighbors[li] > Q[qi])
            qi++;
          else if (l_neighbors[li] < Q[qi])
            li++;
          else {
            Q[q_valid++] = Q[qi];
            qi++;
            li++;
          }
        }
        if (q_valid == 0) last_l = l;
        Q.resize(q_valid);
      }

      for (int ci = i + 1; li < l_neighbors.size() && ci < C.size();) {
        if (!C[ci]->R.empty() || l_neighbors[li] > C[ci]->vc) {
          ci++;
        } else if (l_neighbors[li] < C[ci]->vc) {
          li++;
        } else {
          global_2d_buf_[ci].emplace_back(l);
          li++;
          ci++;
        }
      }
    }

    if (Q.empty()) {
      std::vector<Node *> C_prime;
      int ri = 0;
      while (ri < R.size() && R[ri] < C[i]->vc) {
        C[i]->R.emplace_back(R[ri++]);
      }
      C[i]->R.emplace_back(C[i]->vc);

      for (int ci = i + 1; ci < C.size(); ci++) {
        if (!C[ci]->R.empty()) continue;
        while (ri < R.size() && R[ri] < C[ci]->vc) {
          C[i]->R.emplace_back(R[ri++]);
        }

        if (global_2d_buf_[ci].size() == C[i]->L.size()) {
          C[i]->R.emplace_back(C[ci]->vc);
          if (global_2d_buf_[ci].size() == C[ci]->L.size())
            C[ci]->R.emplace_back(-1);
        } else if (global_2d_buf_[ci].size() >= min_l_size_ &&
                   global_2d_buf_[ci].back() >= last_l) {
          if (global_2d_buf_[ci].size() == C[ci]->L.size())
            C_prime.emplace_back(C[ci]);
          else {
            Node *node = new Node(C[ci]->vc, std::move(global_2d_buf_[ci]));
            create_node_ptrs.emplace_back(node);
            C_prime.emplace_back(node);
          }
        }
        global_2d_buf_[ci].clear();
      }
      while (ri < R.size()) {
        C[i]->R.emplace_back(R[ri]);
        ri++;
      }

      if (C[i]->R.size() >= min_r_size_) {
        if ((++maximal_nodes_) % INTERVAL == 0) {
          printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
        };
        maximum_biclique_.CompareAndSet(C[i]->L, C[i]->R, graph_);
      }
      biclique_find(C[i]->L, C[i]->R, C_prime);
    } else {
      Q.clear();
      C[i]->R.emplace_back(-1);
      for (int ci = i + 1; ci < C.size(); ci++) {
        if (global_2d_buf_[ci].size() == C[ci]->L.size())
          C[ci]->R.emplace_back(-1);
        global_2d_buf_[ci].clear();
      }
    }
  }

  for (auto ptr : create_node_ptrs) delete ptr;
}

AdaMBEFinder::AdaMBEFinder(BiGraph *graph_in, OrderEnum order , const char *name)
    : BicliqueFinder(graph_in, name) {
  order_ = order;
}

void AdaMBEFinder::Execute(int min_l_size, int min_r_size) {
  setup(min_l_size, min_r_size);
  graph_ = new BiGraph(*graph_);
  graph_->Reorder(order_);
  global_2d_buf_.resize(graph_->GetRSize());
  idx_buf_ = std::move(std::vector<int>(graph_->GetRSize(), -1));

  for (cur_v_ = 0; cur_v_ < graph_->GetRSize(); cur_v_++) {
    if (graph_->NeighborsR(cur_v_).size() < min_l_size_) continue;
    if (cur_v_ > 0 && graph_->NeighborsR(cur_v_ - 1) == graph_->NeighborsR(cur_v_)) continue;
    processing_nodes_++;

    if (graph_->NeighborsR(cur_v_).size() > NN) {
      VertexSet &L = graph_->NeighborsR(cur_v_);
      VertexSet R(1, cur_v_);
      std::vector<std::pair<int, VertexSet>> C;

      for (int l : L) {
        auto &l_neighbors = graph_->NeighborsL(l);
        for (int i = l_neighbors.size() - 1; l_neighbors[i] > cur_v_; i--) {
          int r = l_neighbors[i];
          if (idx_buf_[r] < 0) {
            idx_buf_[r] = C.size();
            C.emplace_back(
                std::make_pair(r, std::move(std::vector<int>(1, l))));
          } else {
            C[idx_buf_[r]].second.emplace_back(l);
          }
        }
      }
      std::sort(C.begin(), C.end(),
                [=](std::pair<int, VertexSet> &c0,
                    std::pair<int, VertexSet> &c1) -> bool {
                  return c0.first < c1.first;
                });

      int c_valid = 0;
      for (int i = 0; i < C.size(); i++) {
        idx_buf_[C[i].first] = -1;
        if (C[i].second.size() == L.size()) {
          R.emplace_back(C[i].first);
        } else if (C[i].second.size() >= min_l_size_) {
          if (c_valid != i) C[c_valid] = std::move(C[i]);
          c_valid++;
        }
      }
      C.resize(c_valid);

      if (R.size() >= min_r_size_) {
        if ((++maximal_nodes_) % INTERVAL == 0) {
          printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
        };
        maximum_biclique_.CompareAndSet(L, R, graph_);
      }

      biclique_find(L, R, C);
    } else {
#if NN <= 64
      bitset_t L((~0LLU) >> (64 - graph_->NeighborsR(cur_v_).size()));
#else
      bitset_t L;
      for (int i = 0; i < graph_->NeighborsR(cur_v_).size(); i++) L.set(i);
#endif

      VertexSet R;
      std::vector<std::pair<int, bitset_t>> Q_C;
      int c_start;

      for (int li = 0; li < graph_->NeighborsR(cur_v_).size(); li++) {
        auto &l_neighbors = graph_->NeighborsL(graph_->NeighborsR(cur_v_)[li]);
        for (int r : l_neighbors) {
          if (idx_buf_[r] < 0) {
            idx_buf_[r] = Q_C.size();
            Q_C.emplace_back(std::make_pair(r, bitset_t(1LLU << li)));
          } else {
            Q_C[idx_buf_[r]].second.set(li);
          }
        }
      }
      std::sort(Q_C.begin(), Q_C.end(),
                [=](std::pair<int, bitset_t> &c0, std::pair<int, bitset_t> &c1)
                    -> bool { return c0.first < c1.first; });
      int Q_C_valid = 0;
      for (int i = 0; i < Q_C.size(); i++) {
        idx_buf_[Q_C[i].first] = -1;

        if (Q_C[i].second == L) {
          if (Q_C[i].first == cur_v_) c_start = Q_C_valid;
          R.emplace_back(Q_C[i].first);
        } else if (Q_C[i].second.count() >= min_l_size_) {
          if (Q_C_valid != i) Q_C[Q_C_valid] = std::move(Q_C[i]);
          Q_C_valid++;
        }
      }
      Q_C.resize(Q_C_valid);

      if (R.size() >= min_r_size_) {
        if ((++maximal_nodes_) % INTERVAL == 0) {
          printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
        };
        maximum_biclique_.CompareAndSet(L.count(), R.size());
      }
      biclique_find(L, R, Q_C, c_start);
    }
  }

  finish();
}

void AdaMBEFinder::biclique_find(VertexSet &L, VertexSet &R,
                                 std::vector<std::pair<int, VertexSet>> &C) {
#ifdef EXIT_TIME
  if (get_cur_time() - start_time_ >= EXIT_TIME) {
    finish();
    PrintResult();
    exit(0);
  }
#endif
  VertexSet Q;
  std::vector<int> id_array(C.size(), 0);
  for (int i = 0; i < C.size(); i++) {
    if (C[i].first < 0) continue;
    processing_nodes_++;

    if (C[i].second.size() > NN) {
      VertexSet L_prime = std::move(C[i].second);
      VertexSet R_prime;
      std::vector<std::pair<int, VertexSet>> C_prime;
      int last_l = -1;

      for (int l : L_prime) {
        auto &l_neighbors = graph_->NeighborsL(l);
        int li = 0;

        if (L_prime[0] == l) {  // initialize vertex set Q
          int ri = 0;
          while (l_neighbors[li] < C[i].first) {
            if (ri >= R.size() || l_neighbors[li] < R[ri]) {
              Q.emplace_back(l_neighbors[li]);
              li++;
            } else {
              li++;
              ri++;
            }
          }
        } else if (!Q.empty()) {  // update vertex set Q
          int q_valid = 0;
          for (int qi = 0; li < l_neighbors.size() && qi < Q.size();) {
            if (l_neighbors[li] > Q[qi])
              qi++;
            else if (l_neighbors[li] < Q[qi])
              li++;
            else {
              Q[q_valid++] = Q[qi];
              qi++;
              li++;
            }
          }
          if (q_valid == 0) last_l = l;
          Q.resize(q_valid);
        }

        for (int ci = i + 1;
             li < l_neighbors.size() && ci < C.size();) {  // update candidates
          if (l_neighbors[li] > C[ci].first) {
            ci++;
          } else if (l_neighbors[li] < C[ci].first) {
            li++;
          } else {
            // global_2d_buf_[ci].emplace_back(l);
            // id_array[ci] = -1;

            if (id_array[ci] < 0)
              global_2d_buf_[ci].emplace_back(l);
            else if (C[ci].second[id_array[ci]] == l)
              id_array[ci]++;
            else {
              for (int ii = 0; ii < id_array[ci]; ii++)
                global_2d_buf_[ci].emplace_back(C[ci].second[ii]);
              global_2d_buf_[ci].emplace_back(l);
              id_array[ci] = -1;
            }

            li++;
            ci++;
          }
        }
      }

      if (Q.empty()) {
        int ri = 0;
        while (ri < R.size() && R[ri] < C[i].first)
          R_prime.emplace_back(R[ri++]);
        R_prime.emplace_back(C[i].first);

        for (int ci = i + 1; ci < C.size(); ci++) {
          if (C[ci].first < 0) continue;
          while (ri < R.size() && R[ri] < C[ci].first)
            R_prime.emplace_back(R[ri++]);

          if (id_array[ci] == L_prime.size() ||
              global_2d_buf_[ci].size() == L_prime.size()) {
            R_prime.emplace_back(C[ci].first);
            if (C[ci].second.size() == L_prime.size()) {
              C[ci].first = -1;
              C[ci].second.clear();
            }
          } else if (C[ci].second.size() == id_array[ci]) {
            if (C[ci].second.back() >= last_l)
              C_prime.emplace_back(std::move(C[ci]));
            else
              C[ci].second.clear();
            C[ci].first = -1;
          } else if (id_array[ci] >= min_l_size_ &&
                     C[ci].second[id_array[ci] - 1] >= last_l) {
            VertexSet array(C[ci].second.begin(),
                            C[ci].second.begin() + id_array[ci]);
            C_prime.emplace_back(std::make_pair(C[ci].first, std::move(array)));
          } else if (global_2d_buf_[ci].size() >= min_l_size_ &&
                     global_2d_buf_[ci].back() >= last_l) {
            C_prime.emplace_back(C[ci].first, std::move(global_2d_buf_[ci]));
          }
          id_array[ci] = 0;
          global_2d_buf_[ci].clear();
        }

        while (ri < R.size()) {
          R_prime.emplace_back(R[ri]);
          ri++;
        }

        if (R_prime.size() >= min_r_size_) {
          if ((++maximal_nodes_) % INTERVAL == 0) {
            printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                   get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
          };
          maximum_biclique_.CompareAndSet(L_prime, R_prime, graph_);
        }
        biclique_find(L_prime, R_prime, C_prime);
      } else {
        Q.clear();
        C[i].first = -1;
        C[i].second.clear();
        for (int ci = i + 1; ci < C.size(); ci++) {
          if (id_array[ci] == C[ci].second.size()) {
            C[ci].first = -1;
            C[ci].second.clear();
          }
          id_array[ci] = 0;
          global_2d_buf_[ci].clear();
        }
      }
    } else {
#if NN <= 64
      bitset_t L_prime((~0LLU) >> (64 - C[i].second.size()));
#else
      bitset_t L_prime;
      for (int i = 0; i < C[i].second.size(); i++) L_prime.set(i);
#endif
      VertexSet R_prime;
      std::vector<std::pair<int, bitset_t>> Q_C_prime;
      std::vector<bitset_t> C_bs(C.size(), 0);

      int c_start;

      for (int li = 0; li < C[i].second.size();
           li++) {  // compute local neighbors
        int l = C[i].second[li];
        auto &l_neighbors = graph_->NeighborsL(l);
        int ci = i + 1;

        for (int r : l_neighbors) {
          if (r < C[i].first) {
            if (idx_buf_[r] < 0) {
              idx_buf_[r] = Q_C_prime.size();
              Q_C_prime.emplace_back(std::make_pair(r, bitset_t(1LLU << li)));
            } else {
              Q_C_prime[idx_buf_[r]].second.set(li);
            }
          } else {
            while (ci < C.size() && r > C[ci].first) ci++;
            if (ci >= C.size()) break;
            if (r == C[ci].first) C_bs[ci].set(li);
          }
        }
      }

      int q_c_valid = 0;
      int r_size_before_vc = 0;

      for (int ci = 0; ci < Q_C_prime.size(); ci++) {
        idx_buf_[Q_C_prime[ci].first] = -1;
        if (Q_C_prime[ci].second == L_prime)
          r_size_before_vc++;
        else if (Q_C_prime[ci].second.count() >= min_l_size_) {
          if (q_c_valid != ci) Q_C_prime[q_c_valid] = std::move(Q_C_prime[ci]);
          q_c_valid++;
        }
      }

      if (R.size() >= r_size_before_vc &&
          R[r_size_before_vc - 1] < C[i].first) {  // maximal
        Q_C_prime.resize(q_c_valid);
        R_prime = R;
        R_prime.emplace_back(C[i].first);
        c_start = Q_C_prime.size();
        for (int ci = i + 1; ci < C.size(); ci++) {
          if (C_bs[ci] == L_prime)
            R_prime.emplace_back(C[ci].first);
          else if (C_bs[ci].count() >= min_l_size_)
            Q_C_prime.emplace_back(std::make_pair(C[ci].first, C_bs[ci]));
        }
        if (R_prime.size() >= min_r_size_) {
          if ((++maximal_nodes_) % INTERVAL == 0) {
            printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                   get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
          };
          maximum_biclique_.CompareAndSet(L_prime.count(), R_prime.size());
        }
        biclique_find(L_prime, R_prime, Q_C_prime, c_start);
      }
    }
  }
}

void AdaMBEFinder::biclique_find(bitset_t &L, VertexSet &R,
                                 std::vector<std::pair<int, bitset_t>> &Q_C,
                                 int c_start) {
#ifdef EXIT_TIME
  if (get_cur_time() - start_time_ >= EXIT_TIME) {
    finish();
    PrintResult();
    exit(0);
  }
#endif
  while (c_start < Q_C.size()) {
    processing_nodes_++;
    bitset_t L_prime = Q_C[c_start].second;
    VertexSet R_prime;
    std::vector<std::pair<int, bitset_t>> Q_C_prime;
    int c_start_prime;

    bool is_maximal = true;

    for (int i = 0; i < c_start; i++) {
      bitset_t bs = L_prime & Q_C[i].second;
      if (bs == L_prime) {
        if (L_prime != Q_C[i].second) {
          L_prime = Q_C[i].second;
          Q_C[c_start].second = L_prime;
        }
        is_maximal = false;
      } else if (bs.count() >= min_l_size_) {
        if (is_maximal)
          Q_C_prime.emplace_back(std::make_pair(Q_C[i].first, bs));
      }
      if (Q_C[i].second == bs) Q_C[i].second = 0LLU;
    }

    if (is_maximal) {
      R_prime = R;
      R_prime.emplace_back(Q_C[c_start].first);
      c_start_prime = Q_C_prime.size();
    }

    for (int i = c_start + 1; i < Q_C.size(); i++) {
      bitset_t bs = L_prime & Q_C[i].second;
      if (is_maximal && bs == L_prime)
        R_prime.emplace_back(Q_C[i].first);
      else if (is_maximal && bs.count() >= min_l_size_)
        Q_C_prime.emplace_back(std::make_pair(Q_C[i].first, bs));
      if (Q_C[i].second == bs) Q_C[i].second = 0LLU;
    }

    if (is_maximal) {
      if (R_prime.size() >= min_r_size_) {
        if ((++maximal_nodes_) % INTERVAL == 0) {
          printf("%lld,%lf,%d,%d\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_,cur_v_,graph_->GetRSize());
        };
        maximum_biclique_.CompareAndSet(L_prime.count(), R_prime.size());
      }
      biclique_find(L_prime, R_prime, Q_C_prime, c_start_prime);
    }
    int q_c_valid = 0;

    int c_start_next;
    for (int i = 0; i < Q_C.size(); i++) {
      if (Q_C[i].second != 0) {
        if (q_c_valid != i) Q_C[q_c_valid] = std::move(Q_C[i]);
        q_c_valid++;
      }
      if (i == c_start) c_start_next = q_c_valid;
    }
    Q_C.resize(q_c_valid);
    c_start = c_start_next;
  }
}
