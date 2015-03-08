#ifndef CONTAINERH
#define CONTAINERH

#include <iostream>
#include "config.h"
#include "guest.h"
using namespace std;

class Container : public Guest {
public:
  Container(string n);
  ~Container();

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
