#include "lprocess.h"
#include "monitor.h"
using namespace std;

class SimpleMonitor : public Monitor {
private:
  void checkAndSet(Guest *guest) {
    // CPU allocation
    float usage = guest->getCpuUsage();
    float allocation = guest->getHardCpuShares();
    cout<<"usage "<<usage<<", allocation "<<allocation<<" for guest ";
    cout<<guest->getName()<<endl;

    if(allocation*SAFETY_FACTOR < usage) {
      cout<<"=> usage is about to cross"<<endl;
      guest->setHardCpuShares(allocation*(1+INCREMENT_FACTOR));
    }

    // Network In allocation
    usage = guest->getNetworkInUsage();
    allocation = guest->getNetworkInAllocation();
    cout<<"net in usage "<<usage<<", allocation "<<allocation<<" for guest ";
    cout<<guest->getName()<<endl;

    if(allocation*SAFETY_FACTOR < usage) {
      cout<<"=> net in usage is about to cross"<<endl;
      guest->setNetworkInBW(allocation*(1+INCREMENT_FACTOR));
    }

    // Network Our allocation
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
