#ifndef __UTILITY_H
#define __UTILITY_H
#include <algorithm>
#include <stack>
#include <vector>

/** Timer function **/

/* Returns current time in seconds as a double value
   and the precision can be as accurate as microseconds
   (10^-6 second)
 */
double get_cur_time();

/** Set operation **/

std::vector<int> seq_intersect(const std::vector<int>& v0,
                               const std::vector<int>& v1);
void seq_intersect_local(std::vector<int>& v0, const std::vector<int>& v1);

int seq_intersect_cnt(const std::vector<int>& v0, const std::vector<int>& v1);
std::vector<int> seq_except(const std::vector<int>& v0,
                            const std::vector<int>& v1);
int first_diff_element(const std::vector<int>& v0, const std::vector<int>& v1);

std::vector<int> seq_union(const std::vector<int>& v0,
                           const std::vector<int>& v1);
std::vector<int> seq_intersect_upper(const std::vector<int>& v0,
                                     const std::vector<int>& v1, int bound);
int seq_intersect_cnt_upper(const std::vector<int>& v0,
                            const std::vector<int>& v1, int bound);
int seq_intersect_cnt_lower(const std::vector<int>& v0,
                            const std::vector<int>& v1, int bound);

std::vector<int> seq_except_upper(const std::vector<int>& v0,
                                  const std::vector<int>& v1, int bound);
std::vector<int> seq_union_upper(const std::vector<int>& v0,
                                 const std::vector<int>& v1, int bound);

void seq_intersect_diff(const std::vector<int>& v0, const std::vector<int>& v1,
                        std::vector<int>& res_intersect,
                        std::vector<int>& res_diff);

/** Memory util **/
unsigned int get_proc_vmhwm(unsigned int pid);

/** Bit operation helper **/
unsigned bit_count(unsigned i);
char* extractFileName(char* path);
#endif /* __UTILITY_H */
