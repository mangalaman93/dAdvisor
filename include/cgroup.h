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
  float out_bw;
  int pid;
  void initNetRules();
  void delNetRules();
  string getStringBW(float bw);

public:
  CGroup(string n, string cgroup, string iface);
  ~CGroup();
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
