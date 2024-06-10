#include "ParAdaFinder.h"
#include <chrono>
#include <functional>

std::atomic<uint64_t> total_time(0);

#define MAX_TASK_NUM 960

ParAdaMBEFinder::ParAdaMBEFinder(BiGraph *graph_in, int thread_num_, const char *name)
    : BicliqueFinder(graph_in, name), thread_num(thread_num_), task_num(0) {
  idx_bufs.resize(thread_num_);
  global_2d_bufs.resize(thread_num_);
  for (int i = 0; i < thread_num_; i++) {
    idx_bufs[i] = std::move(std::vector<int>(graph_->GetRSize(), -1));
    global_2d_bufs[i].resize(graph_->GetRSize());
  }
  shared_locks = std::move(std::vector<std::mutex>(thread_num_));
  // std::cout<<"initialize end"<<std::endl;
}

int ParAdaMBEFinder::AcquireFreeIdx() {
  auto start = std::chrono::high_resolution_clock::now();
  while (true) {
    for (int i = 0; i < thread_num; i++) {
      if (shared_locks[i].try_lock()) {
        return i;
      }
    }
    std::cout << "acquire idx fail" << std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  uint64_t sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

void ParAdaMBEFinder::FreeIdx(const int &idx) { shared_locks[idx].unlock(); }

void ParAdaMBEFinder::Execute(int min_l_size, int min_r_size) {
  auto start = std::chrono::high_resolution_clock::now();
  setup(min_l_size, min_r_size);

  auto mp = tbb::global_control::max_allowed_parallelism;
  tbb::global_control gc(mp, thread_num);

  graph_ = new BiGraph(*graph_);
  graph_->Reorder(RInc);

  for (int v = 0; v < graph_->GetRSize(); v++) {
    while (task_num.load() > MAX_TASK_NUM);
    if (graph_->NeighborsR(v).size() < min_l_size_) continue;
    if (v > 0 && graph_->NeighborsR(v - 1) == graph_->NeighborsR(v)) continue;
    processing_nodes_++;
    task_num++;
    tg.run(std::bind<void>([&](int v) {
    int idx = AcquireFreeIdx();
    std::vector<int> &idx_buf_ = idx_bufs[idx];

    if (graph_->NeighborsR(v).size() > NN) {
      VertexSet &L = graph_->NeighborsR(v);
      VertexSet R(1, v);
      std::vector<std::pair<int, VertexSet>> C;

      for (int l : L) {
        auto &l_neighbors = graph_->NeighborsL(l);
        for (int i = l_neighbors.size() - 1; l_neighbors[i] > v; i--) {
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
          printf("%lld,%lf\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_);
        };
        ;
        // maximum_biclique_.CompareAndSet(L, R, graph_);
      }
      FreeIdx(idx);
      biclique_find(std::move(L), std::move(R), std::move(C), 0);
    } else {
      task_num--;
#if NN <= 64
      bitset_t L((~0LLU) >> (64 - graph_->NeighborsR(v).size()));
#else
      bitset_t L;
      for (int i = 0; i < graph_->NeighborsR(v).size(); i++) L.set(i);
#endif

      VertexSet R;
      std::vector<std::pair<int, bitset_t>> Q_C;
      int c_start;

      for (int li = 0; li < graph_->NeighborsR(v).size(); li++) {
        auto &l_neighbors = graph_->NeighborsL(graph_->NeighborsR(v)[li]);
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
          if (Q_C[i].first == v) c_start = Q_C_valid;
          R.emplace_back(Q_C[i].first);
        } else if (Q_C[i].second.count() >= min_l_size_) {
          if (Q_C_valid != i) Q_C[Q_C_valid] = std::move(Q_C[i]);
          Q_C_valid++;
        }
      }
      Q_C.resize(Q_C_valid);

      if (R.size() >= min_r_size_) {
        if ((++maximal_nodes_) % INTERVAL == 0) {
          printf("%lld,%lf\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_);
        };
        ;
        // maximum_biclique_.CompareAndSet(L.count(), R.size());
      }
      FreeIdx(idx);
      biclique_find(std::move(L), std::move(R), std::move(Q_C), c_start);
    }
  }, v));
  }

  tg.wait();
  std::cout << "task:" <<task_num.load() << std::endl;
  finish();
  auto end = std::chrono::high_resolution_clock::now();
  int sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << total_time << std::endl;
}

void ParAdaMBEFinder::biclique_find_serial(VertexSet &L, VertexSet &R,
                                 std::vector<std::pair<int, VertexSet>> &C, int idx) {
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<VertexSet> &global_2d_buf_ = global_2d_bufs[idx];
  std::vector<int> &idx_buf_ = idx_bufs[idx];

  VertexSet Q;
  std::vector<int> id_array(C.size(), 0);
  std::vector<int> c_starts;
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
            printf("%lld,%lf\n", (long long)maximal_nodes_,
                   get_cur_time() - start_time_);
          };
          ;
          // maximum_biclique_.CompareAndSet(L_prime, R_prime, graph_);
        }
        biclique_find_serial(L_prime, R_prime, C_prime, idx);
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
          maximal_nodes_++;
        }
        biclique_find(std::move(L_prime), std::move(R_prime), std::move(Q_C_prime), c_start);
      }
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  int sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

void ParAdaMBEFinder::biclique_find(VertexSet L, VertexSet R,
                                 std::vector<std::pair<int, VertexSet>> C, int depth) {
  int idx = AcquireFreeIdx();
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<VertexSet> &global_2d_buf_ = global_2d_bufs[idx];
  std::vector<int> &idx_buf_ = idx_bufs[idx];

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
          maximal_nodes_++;
          //maximum_biclique_.CompareAndSet(L_prime, R_prime, graph_);
        }
  auto start = std::chrono::high_resolution_clock::now();
        auto tn = task_num++;
        if (tn < MAX_TASK_NUM) {
          tg.run(std::bind(static_cast<void (ParAdaMBEFinder::*) (VertexSet, VertexSet,
                          std::vector<std::pair<int, VertexSet>>, int)>(&ParAdaMBEFinder::biclique_find), this, std::move(L_prime), std::move(R_prime), std::move(C_prime), depth));
        } else {
            /*task_num++;
            tg.run(std::bind<void>([&](VertexSet L_prime, VertexSet R_prime,
                                 std::vector<std::pair<int, VertexSet>> C_prime){
                                              int idx = AcquireFreeIdx();
                                              biclique_find_serial(L_prime, R_prime, C_prime, idx);
                                              FreeIdx(idx);
                                     task_num--;
                                            }, std::move(L_prime), std::move(R_prime), std::move(C_prime)));*/
          task_num--;
          biclique_find_serial(L_prime, R_prime, C_prime, idx);
        }
  auto end = std::chrono::high_resolution_clock::now();
  uint64_t sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
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
            printf("%lld,%lf\n", (long long)maximal_nodes_,
                   get_cur_time() - start_time_);
          };
          ;
          // maximum_biclique_.CompareAndSet(L_prime.count(), R_prime.size());
        }
  auto start = std::chrono::high_resolution_clock::now();
        //tg.run(std::bind(static_cast<void (ParAdaMBEFinder::*) (bitset_t, VertexSet, std::vector<std::pair<int, bitset_t>>, int)>(&ParAdaMBEFinder::biclique_find), this, std::move(L_prime), std::move(R_prime), std::move(Q_C_prime), c_start));
      biclique_find(L_prime, R_prime, Q_C_prime, c_start);
  auto end = std::chrono::high_resolution_clock::now();
  uint64_t sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
      }
    }
  }
  task_num--;
  FreeIdx(idx);
}

void ParAdaMBEFinder::biclique_find(bitset_t L, VertexSet R,
                                 std::vector<std::pair<int, bitset_t>> Q_C,
                                 int c_start) {
  static uint64_t  count = 0, max_time = 0;
  count ++;
  auto start = std::chrono::high_resolution_clock::now();
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
          printf("%lld,%lf\n", (long long)maximal_nodes_,
                 get_cur_time() - start_time_);
        };
        ;
        // maximum_biclique_.CompareAndSet(L_prime.count(), R_prime.size());
      }
      biclique_find(std::move(L_prime), std::move(R_prime), std::move(Q_C_prime), c_start_prime);
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
  auto end = std::chrono::high_resolution_clock::now();
  uint64_t sec = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}
