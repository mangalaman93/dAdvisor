#ifndef CONTAINER_H
#define CONTAINER_H

#include <iomanip>
#include <sstream>
#include "config.h"
#include "guest.h"
#include "utils.h"

class Container : public Guest {
  string id;
  float bw;
  void initNetRules();
  void delNetRules();

public:
  Container(string n, string id);
  ~Container();
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
