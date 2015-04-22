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
  void initNetRules();
  void delNetRules();

public:
  CGroup(string n, string cgroup, string iface);
  ~CGroup();
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
