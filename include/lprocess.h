#ifndef LPROCESS_H
#define LPROCESS_H

#include <iostream>
#include "config.h"
#include "guest.h"
using namespace std;

class LProcess : public Guest {
public:
  LProcess(string n);
  ~LProcess();

  // implementation of virtual functions
  string getType() const;
  float get_cpu_usage();
  float get_soft_cpu_shares();
  float get_hard_cpu_shares();
  int get_pinned_cpus();
  float get_network_in_usage();
  float get_network_out_usage();
  float get_network_in_allocation();
  float get_network_out_allocation();
  void set_soft_cpu_shares(float shares);
  void set_hard_cpu_shares(float shares);
  void set_network_in_bw(float bw);
  void set_network_out_bw(float bw);
};

#endif
