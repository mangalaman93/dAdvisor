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
using namespace std;

// logs current position in file
#define LOG_POS() do {\
  cout<<"line:"<<__LINE__<<" in file:"<<__FILE__<<endl;\
} while(0);

// aborts execution of program
#define SHOULD_NOT_REACH() do {\
  cout<<"should not reach ";\
  LOG_POS();\
  assert(0);\
} while(0);

// Guest Abstract class
class Guest {
  string name;

public:
  Guest(string);
  ~Guest();

  // returns the type of the object
  virtual string getType() const = 0;

  // returns name of the object
  string getName() const;

  // overloading equality operator
  bool equals(const Guest& guest) const;

  // execute system command and abort if not returned expected output
  void systemCmd(string cmd, int ret);

  // get cpu usage, 100 => 1 core
  virtual float get_cpu_usage();

  // get current allocated CPU shares (soft limit) out of 1024
  virtual float get_soft_cpu_shares();

  // get current allocated CPU shares (hard limit) out of 1024
  virtual float get_hard_cpu_shares();

  // maximum number of CPU that can possibly be used by the guest (CPU pinning)
  virtual int get_pinned_cpus();

  // received network traffic usage (rate) in kbytes/sec
  virtual float get_network_in_usage();

  // sent network traffic usage (rate) in kbytes/sec
  virtual float get_network_out_usage();

  // current allocated bandwidth for incoming network traffic (in kbytes/sec)
  virtual float get_network_in_allocation();

  // current allocated bandwidth for outgoing network traffic (in kbytes/sec)
  virtual float get_network_out_allocation();

  // change given value of cpu shares for the container, shares=1024 => 1 core
  virtual void set_soft_cpu_shares(float shares);

  // change current allocated CPU shares (hard limit), shares=1024 => 1 core
  virtual void set_hard_cpu_shares(float shares);

  // change limit on received traffic on network, bw in kbytes/sec
  virtual void set_network_in_bw(float bw);

  // change limit on sent traffic bandwidth, bw in kbytes/sec
  virtual void set_network_out_bw(float bw);
};

#endif
