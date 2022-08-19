// c
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// c++
#include <algorithm>
#include <queue>
#include <set>
#include <vector>
#include <iostream>

// Linux
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>


inline void printDelay(int printNo, const time_t& right_now, const time_t& base) {
  printf("%d, now: %ld, base= %ld, delay= %ld\n",printNo, right_now, base, right_now - base);
}
inline void printTimestamp(int printNo, const time_t& right_now) {
  printf("%d, now: %ld\n",printNo, right_now);
}
inline void printTimestamp(const char* time_info, const time_t& right_now) {
  printf("%s, now: %ld\n",time_info, right_now);
}
inline void writeDelay(FILE *fp, int printNo, const time_t& right_now, const time_t& base) {
   fprintf(fp, "%d, now: %ld, base= %ld, delay= %ld\n",printNo, right_now, base, right_now - base);
}
inline void writeTimestamp(FILE *fp, int printNo, const time_t& right_now) {
   fprintf(fp, "%d, now: %ld\n", printNo, right_now);
}
inline void writeTimestamp(FILE *fp, const char* time_info, const time_t& right_now) {
   fprintf(fp, "%s, now: %ld\n", time_info, right_now);
}
inline void writeQueStatus(FILE *fp, int printNo, const time_t& right_now) {
  printf("%d, now: %ld\n",printNo, right_now);
}