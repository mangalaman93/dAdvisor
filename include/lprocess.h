/* Provides cpu and network control for a linux process,
    uses cpulimit if pid of the process
    uses cgroups if a process(es) cgroup is provided
    uses tc to control incoming and outgoing network traffitc
     on the interface provided
*/

#ifndef LPROCESS_H
#define LPROCESS_H

#include <iostream>
#include <sstream>
#include "config.h"
#include "guest.h"
#include "utils.h"
using namespace std;

class LProcess : public Guest {
  bool is_pid;
  bool is_cgroup;
  int pid;
  string interface;
  string cgroup;
  string filter;
  float in_bw, out_bw;

  void init(string iface, string filter, string cgroup);
  void init(string iface, string filter, int pid);
  void delNetRules();
  void initNetRules();

public:
  LProcess(string n, string filter, string cgroup);
  LProcess(string n, string iface, string filter, string cgroup);
  LProcess(string n, string filter, int pid);
  LProcess(string n, string iface, string filter, int pid);
  ~LProcess();

  // implementation of virtual functions
  string getType() const;
  float getCpuUsage();
  float getSoftCpuShares();
  float getHardCpuShares();
  int getPinnedCpus();
  float getNetworkInUsage();
  float getNetworkOutUsage();
  float getNetworkInAllocation();
  float getNetworkOutAllocation();
  void setSoftCpuShares(float shares);
  void setHardCpuShares(float shares);
  void setNetworkInBW(float bw);
  void setNetworkOutBW(float bw);
};

#endif
