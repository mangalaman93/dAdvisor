#include "lprocess.h"
using namespace std;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <pid>"<<endl;
  } else {
    LProcess lp("app", "eth0", "", atoi(argv[1]));

    while(true){
      sleep(1);
      float usage = lp.getCpuUsage();
      float netin = lp.getNetworkInUsage();
      float netout = lp.getNetworkOutUsage();
      cout<<usage<<"\t"<<netin<<"\t"<<netout<<endl;
    }
  }

  return 0;
}
