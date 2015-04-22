#ifndef LPROCESS_H
#define LPROCESS_H

#include <sstream>
#include "config.h"
#include "guest.h"
#include "utils.h"
using namespace std;

class LProcess : public Guest {
  int pid;
  int shares;
  string interface;
  int last_handle;
  float bw;
  void initNetRules();
  void delNetRules();

public:
  LProcess(string n, int pid, string iface);
  ~LProcess();
  string getType() const;
  unsigned long getCPUCumUsage();
  unsigned int getSoftCPUShares();
  unsigned int getHardCPUShares();
  unsigned int getPinnedCPUs();
  void setSoftCPUShares(unsigned int shares);
  void setHardCPUShares(unsigned int shares);
  unsigned long getNetworkOutCumUsage();
  float getNetworkOutAllocation();
  void setNetworkOutBW(float bw);
};

#endif
