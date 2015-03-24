#include "simplemonitor.cpp"
using namespace std;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <pid>"<<endl;
  } else {
    LProcess lp("server", "eth0", "", atoi(argv[1]));
    lp.setHardCpuShares(100);
    lp.setNetworkInBW(40000);
    lp.setNetworkOutBW(1000);
    sleep(2);

    SimpleMonitor monitor;
    monitor.addGuest(&lp);
    monitor.run();
  }

  return 0;
}
