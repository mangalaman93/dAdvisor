#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <cstdlib>
#include <iostream>
using namespace std;

// logs current position in file
#define LOG_POS() do {\
  cout<<"line:"<<__LINE__<<" in file:"<<__FILE__<<endl;\
} while(0);

// aborts execution of program
#define SHOULD_NOT_REACH() do {\
  cout<<"should not reach ";\
  LOG_POS();\
  assert(0);\
} while(0);

class Utils {
public:
  static int systemCmd(const string& cmd);
  static void systemCmd(const string& cmd, int ret);
  static int systemCmd(const string& cmd, string& out);
  static void systemCmd(const string& cmd, string& out, int ret);
};

#endif
