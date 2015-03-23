#include "lprocess.h"
#include "monitor.h"
using namespace std;

class SimpleMonitor : public Monitor {
private:
  void checkAndSet(Guest *guest) {
    float usage = guest->getCpuUsage();
    float allocation = guest->getHardCpuShares();
    cout<<"usage "<<usage<<", allocation "<<allocation<<" for guest ";
    cout<<guest->getName()<<endl;

    if(allocation*SAFETY_FACTOR < usage) {
      cout<<"=> usage is about to cross allocation for guest"<<endl;
      guest->setHardCpuShares(allocation*(1+INCREMENT_FACTOR));
    }

    return;
  }

public:
  SimpleMonitor() {}
  ~SimpleMonitor() {}
};

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <pid>"<<endl;
  } else {
    LProcess lp("voip", "eth0", "", atoi(argv[1]));
    lp.setHardCpuShares(10);

    SimpleMonitor monitor;
    monitor.addGuest(&lp);
    monitor.run();
  }

  return 0;
}
