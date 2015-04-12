#include "lprocess.h"
#include "monitor.h"
using namespace std;

class SimpleMonitor : public Monitor {
private:
  void checkAndSet(Guest *guest) {
    // CPU allocation
    float usage = guest->getCPUUsage();
    float allocation = guest->getHardCPUShares();
    cout<<"usage "<<usage<<", allocation "<<allocation<<" for guest ";
    cout<<guest->getName()<<endl;

    if(allocation*SAFETY_FACTOR < usage) {
      cout<<"=> usage is about to cross"<<endl;
      guest->setHardCPUShares(allocation*(1+INCREMENT_FACTOR));
    }

    // Network Out allocation
    usage = guest->getNetworkOutUsage();
    allocation = guest->getNetworkOutAllocation();
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
