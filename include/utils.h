#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ifaddrs.h>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

// logs current position in file
#define LOG_POS() do {                                     \
  cout<<"line:"<<__LINE__<<" in file:"<<__FILE__<<endl;    \
} while(0);

// aborts execution of program
#define SHOULD_NOT_REACH() do {                            \
  cout<<"should not reach ";                               \
  LOG_POS();                                               \
  assert(0);                                               \
} while(0);

class Utils {
public:
  static int systemCmd(const string& cmd);
  static void systemCmd(const string& cmd, int ret);
  static int systemCmd(const string& cmd, string& out);
  static void systemCmd(const string& cmd, string& out, int ret);
  static string getIPAddr(string interface);
  static void readFile(const string& path, string& content);
  static void writeFile(const string& path, const string& content);
  static string getStringBW(float bw);
};

#endif
