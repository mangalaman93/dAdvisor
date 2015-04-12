#include "simplemonitor.cpp"
using namespace std;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <pid>"<<endl;
  } else {
    LProcess lp("voip", atoi(argv[1]), "eth0");
    lp.setHardCPUShares(10);
    lp.setNetworkOutBW(10);
    sleep(2);

    SimpleMonitor monitor;
    monitor.addGuest(&lp);
    monitor.run();
  }

  return 0;
}
