#ifndef LPROCESS_H
#define LPROCESS_H

#include <sstream>
#include "config.h"
#include "guest.h"
#include "utils.h"
using namespace std;

class LProcess : public Guest {
  int pid;
  float shares;
  string interface;
  int last_handle;
  float out_bw;
  void initNetRules();
  void delNetRules();

public:
  LProcess(string n, int pid, string iface);
  ~LProcess();
  string getType() const;
  unsigned long getCPUCumUsage();
  float getSoftCPUShares();
  float getHardCPUShares();
  int getPinnedCPUs();
  void setSoftCPUShares(float shares);
  void setHardCPUShares(float shares);
  unsigned long getNetworkOutCumUsage();
  float getNetworkOutBW();
  void setNetworkOutBW(float bw);
};

#endif
