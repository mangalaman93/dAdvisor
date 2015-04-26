/* machine[name/id][resource] */

#ifndef MACHINE_H
#define MACHINE_H

#include <map>
using namespace std;

#define CPU 0
#define NET_OUT 1

// for map
struct CharCompare : public binary_function<const char*, const char*, bool> {
public:
  bool operator() (const char* str1, const char* str2) const {
   return strcmp(str1, str2) < 0;
 }
};

const int SNORT_CAPACITY[1] = {100};
const int SIPP_CLIENT_CAPACITY[1] = {200};
const int SIPP_SERVER_CAPACITY[1] = {200};
const map<const char*, const int*, CharCompare> machine_capacity = {{"snort", SNORT_CAPACITY},
  {"sipp-client", SIPP_CLIENT_CAPACITY}, {"sipp-server", SIPP_SERVER_CAPACITY}};

#endif
