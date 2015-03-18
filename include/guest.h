/* Guest abstract class for controlling monitoring
   processes and various virutalized platforms,
   provides functions to set/get network and CPU allocation/usage,
   network bandwidth are in kbytes/sec
*/

#ifndef GUEST_H
#define GUEST_H

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include "utils.h"
using namespace std;

// Guest Abstract class
class Guest {
  string name;

public:
  Guest(string);
  virtual ~Guest();

  // returns the type of the object
  virtual string getType() const = 0;

  // returns name of the object
  string getName() const;

  // overloading equality operator
  bool equals(const Guest& guest) const;

  // execute system command and abort if not returned expected output
  string systemCmd(const string& cmd, int ret);

  // get cpu usage, 100 => 1 core
  virtual float getCpuUsage();

  // get current allocated CPU shares (soft limit) out of 100
  virtual float getSoftCpuShares();

  // get current allocated CPU shares (hard limit) out of 100
  virtual float getHardCpuShares();

  // maximum number of CPU that can possibly be used by the guest (CPU pinning)
  virtual int getPinnedCpus();

  // received network traffic usage (rate) in kbytes/sec
  virtual float getNetworkInUsage();

  // sent network traffic usage (rate) in kbytes/sec
  virtual float getNetworkOutUsage();

  // current allocated bandwidth for incoming network traffic (in kbytes/sec)
  virtual float getNetworkInAllocation();

  // current allocated bandwidth for outgoing network traffic (in kbytes/sec)
  virtual float getNetworkOutAllocation();

  // change given value of cpu shares for the container, shares=100 => 1 core
  virtual void setSoftCpuShares(float shares);

  // change current allocated CPU shares (hard limit), shares=100 => 1 core
  virtual void setHardCpuShares(float shares);

  // change limit on received traffic on network, bw in kbytes/sec
  virtual void setNetworkInBW(float bw);

  // change limit on sent traffic bandwidth, bw in kbytes/sec
  virtual void setNetworkOutBW(float bw);
};

#endif
