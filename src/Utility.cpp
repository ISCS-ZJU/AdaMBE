#include "Utility.h"
#include <errno.h>

#include <string.h>

#include <unordered_set>
#ifdef _MSC_VER
#include <psapi.h>
#include <windows.h>
#define fopen64 fopen
double get_cur_time() {
  LARGE_INTEGER nFreq;
  LARGE_INTEGER nTime;
  QueryPerformanceFrequency(&nFreq);
  QueryPerformanceCounter(&nTime);
  double time = (double)nTime.QuadPart / (double)nFreq.QuadPart;
  return time;
}

#else
#include <stdio.h>
#include <sys/time.h> /* gettimeofday */
double get_cur_time() {
  struct timeval tv;
  struct timezone tz;
  double cur_time;
  gettimeofday(&tv, &tz);
  cur_time = tv.tv_sec + tv.tv_usec / 1000000.0;
  return cur_time;
}
#define VMHWM_LINE 21
unsigned int get_proc_vmhwm(unsigned int pid) {
  char file_name[64] = {0};
  FILE *fd;
  char line_buff[512] = {0};
  sprintf(file_name, "/proc/%d/status", pid);

  fd = fopen(file_name, "r");
  if (nullptr == fd) {
    return 0;
  }

  char name[64];
  int vmhwm;
  for (int i = 0; i < VMHWM_LINE - 1; i++) {
    if (fgets(line_buff, sizeof(line_buff), fd) == NULL) {
      fprintf(stderr, "Error reading file: %s\n", strerror(errno));
    }
  }

  while (true) {
    if (fgets(line_buff, sizeof(line_buff), fd) == NULL) {
      fprintf(stderr, "Error reading file: %s\n", strerror(errno));
    }
    sscanf(line_buff, "%s %d", name, &vmhwm);
    if (!strcmp(name, "VmHWM:")) {
      break;
    }
  }

  fclose(fd);

  return vmhwm;
}
#endif

std::vector<int> seq_intersect(const std::vector<int> &v0,
                               const std::vector<int> &v1) {
  std::vector<int> res;

  if (v0.size() > 8 * v1.size()) {
    auto iter_begin = v0.begin();
    auto iter_end = v0.end();

    for (int v : v1) {
      auto iter_aim = std::lower_bound(iter_begin, iter_end, v);
      if (iter_aim == v0.end())
        break;
      else if (*iter_aim == v)
        res.emplace_back(v);
      iter_begin = iter_aim;
    }
  } else if (v0.size() * 8 < v1.size()) {
    auto iter_begin = v1.begin();
    auto iter_end = v1.end();

    for (int v : v0) {
      auto iter_aim = std::lower_bound(iter_begin, iter_end, v);
      if (iter_aim == v1.end())
        break;
      else if (*iter_aim == v)
        res.emplace_back(v);
      iter_begin = iter_aim;
    }
  } else {
    for (auto it0 = v0.begin(), it1 = v1.begin();
         it0 != v0.end() && it1 != v1.end();) {
      if (*it0 == *it1) {
        res.emplace_back(*it0);
        it0++;
        it1++;
      } else if (*it0 > *it1)
        it1++;
      else
        it0++;
    }
  }
  return res;
}

void seq_intersect_local(std::vector<int> &v0, const std::vector<int> &v1) {
  int v0_valid = 0;
  auto it = v1.begin();
  for (int i = 0; i < v0.size() && it != v1.end(); i++) {
    while (it != v1.end() && *it < v0[i]) it++;
    if (it != v1.end() && *it == v0[i]) {
      if (i != v0_valid) v0[v0_valid] = v0[i];
      v0_valid++;
    }
  }
  v0.resize(v0_valid);
}

int seq_intersect_cnt(const std::vector<int> &v0, const std::vector<int> &v1) {
  int cnt = 0;
  for (auto it0 = v0.begin(), it1 = v1.begin();
       it0 != v0.end() && it1 != v1.end();) {
    if (*it0 == *it1) {
      cnt++;
      it0++;
      it1++;
    } else if (*it0 > *it1)
      it1++;
    else
      it0++;
  }
  return cnt;
}

std::vector<int> seq_except(const std::vector<int> &v0,
                            const std::vector<int> &v1) {
  std::vector<int> res;
  for (auto it0 = v0.begin(), it1 = v1.begin(); it0 != v0.end();) {
    if (it1 == v1.end() || *it0 < *it1) {
      res.emplace_back(*it0);
      it0++;
    } else if (*it0 == *it1) {
      it0++;
      it1++;
    } else {
      it1++;
    }
  }
  return res;
}

int first_diff_element(const std::vector<int> &v0, const std::vector<int> &v1) {
  for (int i = 0; i < std::min(v0.size(), v1.size()); i++) {
    if (v0[i] != v1[i]) return v0[i];
  }
  return (v0.size() > v1.size()) ? v0[v1.size()] : -1;
}

std::vector<int> seq_union(const std::vector<int> &v0,
                           const std::vector<int> &v1) {
  std::vector<int> res;
  for (auto it0 = v0.begin(), it1 = v1.begin();
       it0 != v0.end() || it1 != v1.end();) {
    if (it1 == v1.end())
      res.emplace_back(*it0++);
    else if (it0 == v0.end())
      res.emplace_back(*it1++);
    else if (*it0 == *it1) {
      res.emplace_back(*it0);
      it0++;
      it1++;
    } else if (*it0 > *it1) {
      res.emplace_back(*it1);
      it1++;
    } else {
      res.emplace_back(*it0);
      it0++;
    }
  }
  return res;
}

std::vector<int> seq_intersect_upper(const std::vector<int> &v0,
                                     const std::vector<int> &v1, int bound) {
  std::vector<int> res;
  for (auto it0 = std::upper_bound(v0.begin(), v0.end(), bound),
            it1 = std::upper_bound(v1.begin(), v1.end(), bound);
       it0 != v0.end() && it1 != v1.end();) {
    if (*it0 == *it1) {
      res.emplace_back(*it0);
      it0++;
      it1++;
    } else if (*it0 > *it1)
      it1++;
    else
      it0++;
  }
  return res;
}

int seq_intersect_cnt_upper(const std::vector<int> &v0,
                            const std::vector<int> &v1, int bound) {
  int cnt = 0;
  for (auto it0 = std::upper_bound(v0.begin(), v0.end(), bound),
            it1 = std::upper_bound(v1.begin(), v1.end(), bound);
       it0 != v0.end() && it1 != v1.end();) {
    if (*it0 == *it1) {
      cnt++;
      it0++;
      it1++;
    } else if (*it0 > *it1)
      it1++;
    else
      it0++;
  }
  return cnt;
}

int seq_intersect_cnt_lower(const std::vector<int> &v0,
                            const std::vector<int> &v1, int bound) {
  int cnt = 0;
  for (auto it0 = v0.begin(), it1 = v1.begin();
       it0 != v0.end() && it1 != v1.end() && *it0 < bound && *it1 < bound;) {
    if (*it0 == *it1) {
      cnt++;
      it0++;
      it1++;
    } else if (*it0 > *it1)
      it1++;
    else
      it0++;
  }
  return cnt;
}

std::vector<int> seq_except_upper(const std::vector<int> &v0,
                                  const std::vector<int> &v1, int bound) {
  std::vector<int> res;
  for (auto it0 = std::upper_bound(v0.begin(), v0.end(), bound),
            it1 = std::upper_bound(v1.begin(), v1.end(), bound);
       it0 != v0.end();) {
    if (it1 == v1.end() || *it0 < *it1) {
      res.emplace_back(*it0);
      it0++;
    } else if (*it0 == *it1) {
      it0++;
      it1++;
    } else {
      it1++;
    }
  }
  return res;
}

std::vector<int> seq_union_upper(const std::vector<int> &v0,
                                 const std::vector<int> &v1, int bound) {
  std::vector<int> res;
  for (auto it0 = std::upper_bound(v0.begin(), v0.end(), bound),
            it1 = std::upper_bound(v1.begin(), v1.end(), bound);
       it0 != v0.end() || it1 != v1.end();) {
    if (it1 == v1.end())
      res.emplace_back(*it0++);
    else if (it0 == v0.end())
      res.emplace_back(*it1++);
    else if (*it0 == *it1) {
      res.emplace_back(*it0);
      it0++;
      it1++;
    } else if (*it0 > *it1) {
      res.emplace_back(*it1);
      it1++;
    } else {
      res.emplace_back(*it0);
      it0++;
    }
  }
  return res;
}

void seq_intersect_diff(const std::vector<int> &v0, const std::vector<int> &v1,
                        std::vector<int> &res_intersect,
                        std::vector<int> &res_diff) {
  for (auto it0 = v0.begin(), it1 = v1.begin(); it0 != v0.end();) {
    if (it1 == v1.end() || *it0 < *it1) {
      res_diff.emplace_back(*it0);
      it0++;
    } else if (*it0 == *it1) {
      res_intersect.emplace_back(*it0);
      it0++;
      it1++;
    } else {
      it1++;
    }
  }
}

unsigned bit_count(unsigned i) {
  i = (i & 0x55555555) + ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  i = (i & 0x0F0F0F0F) + ((i >> 4) & 0x0F0F0F0F);
  i = (i * (0x01010101) >> 24);
  return i;
}

char *extractFileName(char *path) {
  size_t len = strlen(path);
  const char *start = path;
  const char *end = path + len;

  // Find last slash position
  for (const char *p = end - 1; p >= start; --p) {
    if (*p == '/') {
      start = p + 1;
      break;
    }
  }

  // Find .adj position
  for (const char *p = start; p < end; ++p) {
    if (*p == '.') {
      if (strncmp(p, ".adj", 4) == 0) {
        size_t extractedLen = p - start;
        char *extractedName = new char[extractedLen + 1];
        strncpy(extractedName, start, extractedLen);
        extractedName[extractedLen] = '\0';
        return extractedName;
      } else {
        break;
      }
    }
  }

  return nullptr;
}
