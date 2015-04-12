#ifndef CGROUP_H
#define CGROUP_H

#include <iomanip>
#include <sstream>
#include "config.h"
#include "guest.h"
#include "utils.h"

class CGroup : public Guest {
  string cgroup;
  string interface;
  string filter;
  int last_handle;
  float bw;
  int pid;

  int getCumCPUUsage();
  void initNetRules();
  void delNetRules();
  float getCumNetworkUsage(string cmd);

public:
  CGroup(string n, string cgroup, string iface);
  ~CGroup();

  string getType() const = 0;
  float getCPUUsage();
  float getSoftCPUShares();
  float getHardCPUShares();
  int getPinnedCPUs();
  void setSoftCPUShares(float shares);
  void setHardCPUShares(float shares);
  float getNetworkOutUsage();
  float getNetworkOutAllocation();
  void setNetworkOutBW(float bw);
};

#endif
