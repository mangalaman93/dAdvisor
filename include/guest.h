/* Guest abstract class for controlling monitoring
   processes and various (non)virutalized platforms */

#ifndef GUEST_H
#define GUEST_H

#include <string>
#include <utils.h>
using namespace std;

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
  virtual bool equals(const Guest& guest) const;

  // received network traffic cummulative usage (bytes)
  virtual unsigned long getNetworkInCumUsage();

  // sent network traffic usage (rate) in bytes/sec
  virtual unsigned long getNetworkOutCumUsage();

  // current allocated bandwidth for incoming network traffic (in bytes/sec)
  virtual float getNetworkInBW();

  // current allocated bandwidth for outgoing network traffic (in bytes/sec)
  virtual float getNetworkOutBW();

  // change limit on received traffic on network, bw in kbytes/sec
  virtual void setNetworkInBW(float bw);

  // change limit on sent traffic bandwidth, bw in kbytes/sec
  virtual void setNetworkOutBW(float bw);

  // get cpu cummulative usage
  virtual unsigned long getCPUCumUsage();

  // get current allocated CPU shares out of 100
  virtual float getSoftCPUShares();

  // get current allocated CPU shares (hard limit) out of 100
  virtual float getHardCPUShares();

  // maximum number of CPU that can possibly be used by the guest (CPU pinning)
  virtual int getPinnedCPUs();

  // change given value of cpu shares for the object, shares=100 => 1 core
  virtual void setSoftCPUShares(float shares);

  // change current allocated CPU shares (hard limit), shares=100 => 1 core
  virtual void setHardCPUShares (float shares);

  // change maximum number of CPU that can possibly be used by the guest (CPU pinning)
  virtual void setPinnedCPUs(int num_cpus);
};

#endif
