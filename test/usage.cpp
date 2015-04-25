#include "lprocess.h"
using namespace std;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout<<"usage: "<<argv[0]<<" <pid>"<<endl;
  } else {
    LProcess lp("app", atoi(argv[1]), "eth0");

    while(true){
      float usage = lp.getCPUCumUsage();
      float netout = lp.getNetworkOutCumUsage();
      cout<<usage<<"\t"<<netout<<endl;
    }
  }

  return 0;
}
