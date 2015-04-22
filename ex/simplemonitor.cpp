#include <sys/time.h>
#include "lprocess.h"
#include "monitor.h"
using namespace std;

class SimpleMonitor : public Monitor {
  unsigned long cpu_usage;
  unsigned long net_usage;
  struct timeval cpu_time, net_time;

private:
  void checkAndSet(Guest *guest) {
    unsigned long old_usage;
    float allocation;
    float usage;
    struct timeval old_time;
    double elapsed_time;

    // CPU allocation
    old_usage = cpu_usage;
    old_time = cpu_time;
    allocation = guest->getHardCPUShares();
    cpu_usage = guest->getCPUCumUsage();
    gettimeofday(&cpu_time, NULL);
    elapsed_time = (cpu_time.tv_sec - old_time.tv_sec) * 1000.0;
    elapsed_time += (cpu_time.tv_usec - old_time.tv_usec) / 1000.0;
    usage = (cpu_usage-old_usage)/elapsed_time;
    cout<<"usage "<<usage<<", allocation "<<allocation<<" for guest ";
    cout<<guest->getName()<<endl;

    if(allocation*SAFETY_FACTOR < usage) {
      cout<<"=> usage is about to cross"<<endl;
      guest->setHardCPUShares(allocation*(1+INCREMENT_FACTOR));
    }

    // Network Out allocation
    old_usage = net_usage;
    old_time = net_time;
    allocation = guest->getNetworkOutAllocation();
    net_usage = guest->getNetworkOutCumUsage();
    gettimeofday(&net_time, NULL);
    elapsed_time = (net_time.tv_sec - old_time.tv_sec) * 1000.0;
    elapsed_time += (net_time.tv_usec - old_time.tv_usec) / 1000.0;
    usage = (net_usage-old_usage)/elapsed_time;
    cout<<"net Out usage "<<usage<<", allocation "<<allocation<<" for guest ";
    cout<<guest->getName()<<endl;

    if(allocation*SAFETY_FACTOR < usage) {
      cout<<"=> net Out usage is about to cross"<<endl;
      guest->setNetworkOutBW(allocation*(1+INCREMENT_FACTOR));
    }

    return;
  }

public:
  SimpleMonitor() {}
  ~SimpleMonitor() {}
};
