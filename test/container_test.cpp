/* Example file to create a container object,
   Run following commands:
docker run -it --rm --name cont fedora/stress /bin/sh
stress -c 2
docker run -it --rm --name cont gdanii/iperf /bin/sh
iperf -c 172.17.42.1
 */

#include <sys/time.h>
#include "container.h"
#include "monitor.h"
using namespace std;

class SimpleMonitor : public Monitor {
  unsigned long cpu_usage, net_usage;
  bool cpu_flag, net_flag;
  struct timeval cpu_time, net_time;

private:
  void checkAndSet() {
    unsigned long old_usage;
    float allocation;
    float usage;
    struct timeval old_time;
    double elapsed_time;

    // CPU
    cout<<"----------------------CPU------------------------"<<endl;
    if(cpu_flag) {
      old_usage = cpu_usage;
      old_time = cpu_time;

      cpu_usage = gobj->getCPUCumUsage();
      gettimeofday(&cpu_time, NULL);
      elapsed_time = (cpu_time.tv_sec - old_time.tv_sec);
      elapsed_time += (cpu_time.tv_usec - old_time.tv_usec) / 1000000.0;

      allocation = gobj->getHardCPUShares();
      usage = (cpu_usage-old_usage)/elapsed_time;
      cout<<"usage "<<usage<<", allocation "<<allocation<<" for guest ";
      cout<<gobj->getName()<<endl;

      if(allocation*SAFETY_FACTOR < usage) {
        cout<<"=> usage is about to cross"<<endl;
        gobj->setHardCPUShares(allocation*(1+INCREMENT_FACTOR));
      }
    }

    // Sent network traffic
    cout<<"----------------------NET------------------------"<<endl;
    if(net_flag) {
      old_usage = net_usage;
      old_time = net_time;

      net_usage = gobj->getNetworkOutCumUsage();
      gettimeofday(&net_time, NULL);
      elapsed_time = (net_time.tv_sec - old_time.tv_sec);
      elapsed_time += (net_time.tv_usec - old_time.tv_usec) / 1000000.0;

      allocation = gobj->getNetworkOutBW();
      usage = (net_usage-old_usage)/elapsed_time;
      cout<<"net out usage "<<usage<<", allocation "<<allocation<<" for guest ";
      cout<<gobj->getName()<<endl;

      if(allocation*SAFETY_FACTOR < usage) {
        cout<<"=> net out usage is about to cross"<<endl;
        gobj->setNetworkOutBW(allocation*(1+INCREMENT_FACTOR));
      }
    }
  }

public:
  SimpleMonitor(Guest *guest, bool cpu_flag, bool net_flag) : Monitor(guest) {
    this->cpu_flag = cpu_flag;
    this->net_flag = net_flag;
    this->cpu_usage = guest->getCPUCumUsage();
    this->net_usage = guest->getNetworkOutCumUsage();
  }

  ~SimpleMonitor() {}
};

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <container-id>"<<endl;
  } else {
    Container container("test-container", argv[1]);
    container.setHardCPUShares(50);
    container.setNetworkOutBW(200*1024);
    sleep(2);

    SimpleMonitor monitor(&container, true, true);
    monitor.run();
  }

  return 0;
}
