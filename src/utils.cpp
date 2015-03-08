#include "utils.h"

int Utils::systemCmd(const string& cmd) {
  FILE* pipe{ popen(cmd.c_str(), "r")};

  // the exit status is in top 16 bits
  return (pclose(pipe)/256);
}

void Utils::systemCmd(const string& cmd, int ret) {
  FILE* pipe{ popen(cmd.c_str(), "r")};
  int r = (pclose(pipe)/256);
  if(ret != r) {
    cout<<"unsuccessful to run command: \""<<cmd<<"\" ";
    LOG_POS();
    assert(0);
  }
}

int Utils::systemCmd(const string& cmd, string& out) {
  FILE* pipe{ popen(cmd.c_str(), "r")};

  char buffer[256];
  while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
    string file = buffer;
    out += file.substr(0, file.size()-1);
  }

  return (pclose(pipe)/256);
}

void Utils::systemCmd(const string& cmd, string& out, int ret) {
  int r = Utils::systemCmd(cmd, out);
  if(ret != r) {
    cout<<"unsuccessful to run command: \""<<cmd<<"\" ";
    LOG_POS();
    assert(0);
  }
}
