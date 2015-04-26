#ifndef CONTAINER_H
#define CONTAINER_H

#include <iomanip>
#include <sstream>
#include "config.h"
#include "guest.h"
#include "utils.h"

class Container : public Guest {
  string id;
  float out_bw;
  void initNetRules();
  void delNetRules();

public:
  Container(string n, string id);
  ~Container();
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
