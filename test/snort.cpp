#include "simplemonitor.cpp"
using namespace std;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <pid>"<<endl;
  } else {
    LProcess lp("snort", atoi(argv[1]), "eth0");
    lp.setHardCPUShares(20);
    lp.setNetworkOutBW(1500);
    sleep(2);

    SimpleMonitor monitor(true, true);
    monitor.addGuest(&lp);
    monitor.run();
  }

  return 0;
}
