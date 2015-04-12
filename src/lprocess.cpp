#include "lprocess.h"

void LProcess::delNetRules() {
  stringstream ss;
  ss<<"tc qdisc add dev "<<this->interface<<" root handle 1:0 htb default ";
  ss<<this->last_handle;
  Utils::systemCmd(ss.str(), 0);
}

void LProcess::initNetRules() {
  stringstream ss;
  ss<<"tc qdisc del dev "<<this->interface<<" root";
  Utils::systemCmd(ss.str());
}

LProcess::LProcess(string n, int pid, string iface) : Guest(n) {
  this->pid = pid;
  this->interface = iface;
  this->last_handle = DEFAULT_HANDLE;
  this->delNetRules();
  this->initNetRules();

  // other settings
  this->bw = -1;
  this->shares = -1;
}

LProcess::~LProcess() {
  this->delNetRules();
}

string LProcess::getType() const {
  return TYPE_LPROCESS;
}

float LProcess::getCPUUsage() {
  stringstream ss;
  ss<<"top -b -w 60 -d 1 -p "<<this->pid<<" -n 2 | grep ";
  ss<<this->pid<<" | tail -n 1 | awk '{print $(NF-1)}'";
  string result;
  Utils::systemCmd(ss.str(), result);
  return atof(result.c_str());
}

float LProcess::getHardCPUShares() {
  return this->shares;
}

float LProcess::getSoftCPUShares() {
  return this->shares;
}

int LProcess::getPinnedCPUs() {
  string cpu_str;
  stringstream ss;
  ss<<"taskset -c -p "<<this->pid;
  string result;
  Utils::systemCmd(ss.str(), result);

  int index;
  for(unsigned int i=0; i<result.length(); i++) {
    if(result[i] == ':') {
      index = i;
      break;
    }
  }

  index += 2;
  for(unsigned int i=index; i<result.length(); i++) {
    cpu_str.push_back(result[i]);
  }

  bool flag = false;
  int total_cpus = 0;
  cpu_str.push_back(',');
  for(unsigned int i=0; i<cpu_str.length(); i++) {
    char num[5];
    int j=0;

    while(true) {
      if(cpu_str[i] == ',') {
        if(flag) {
          num[j] = '\0';
          total_cpus += atoi(num);
          flag = false;
        } else {
          total_cpus += 1;
        }

        break;
      } else if(cpu_str[i] == '-') {
        num[j] = '\0';
        total_cpus -= (atoi(num) - 1);
        flag = true;
        break;
      } else if(cpu_str[i] == '\0') {
        break;
      } else {
        num[j] = cpu_str[i];
        j++;
      }

      i++;
    }
  }

  return total_cpus;
}

void LProcess::setSoftCPUShares(float shares) {
  this->setHardCPUShares(shares);
}

void LProcess::setHardCPUShares(float shares) {
  if(shares > this->getPinnedCPUs()*100) {
    cout<<"Error: trying to allocate shares more than available!"<<endl;
    return;
  }

  this->shares = shares;
  Utils::systemCmd("sudo killall -9 cpulimit");

  int limit = (int)shares;
  stringstream ss;
  ss<<"sudo cpulimit -bz -p "<<this->pid<<" -l "<<limit;
  Utils::systemCmd(ss.str());
}

float LProcess::getNetworkOutUsage() {
  stringstream ss;
  ss<<NET_FOLDER<<this->interface<<+"statistics/tx_bytes";

  string result;
  Utils::readFile(ss.str(), result);
  int before_usage = atoi(result.c_str());

  usleep(USAGE_CHECK_PERIOD);
  result.clear();
  Utils::readFile(ss.str(), result);
  int after_usage = atoi(result.c_str());
  return ((after_usage-before_usage)*(1000000/USAGE_CHECK_PERIOD))/1024;
}

float LProcess::getNetworkOutAllocation() {
  return bw;
}

void LProcess::setNetworkOutBW(float bw) {
  if(this->bw == -1) {
    this->bw = bw;

    stringstream ss;
    ss<<"tc class add dev "<<this->interface;
    ss<<" parent 1: classid 1:"<<this->last_handle<<" htb rate ";
    ss<<this->bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);
  } else {
    this->bw = bw;
    stringstream ss;
    ss<<"tc class change dev "<<this->interface;
    ss<<" parent 1: classid 1:"<<this->last_handle<<" htb rate "<<bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);
  }
}
