#include "cgroup.h"

void CGroup::initNetRules() {
  stringstream ss;
  ss<<"tc qdisc add dev "<<this->interface<<" root handle 1:0 htb default ";
  ss<<this->last_handle;
  Utils::systemCmd(ss.str(), 0);
}

void CGroup::delNetRules() {
  stringstream ss;
  ss<<"tc qdisc del dev "<<this->interface<<" root";
  Utils::systemCmd(ss.str());
}

CGroup::CGroup(string n, string cgroup, string iface)
    : Guest(n) {
  this->cgroup = cgroup;
  this->interface = iface;
  this->last_handle = DEFAULT_HANDLE;
  this->delNetRules();
  this->initNetRules();

  // other settings
  this->bw = -1;

  // get the pid of the process running for network metrics
  stringstream ss;
  ss<<"cat "<<BASE_URL<<NET_CLASS<<"/"<<this->cgroup<<"/"<<TASKS_FILE;
  ss<<" | head -1";
  string result;
  Utils::systemCmd(ss.str(), result);
  this->pid = atoi(result.c_str());
}

CGroup::~CGroup() {
  this->delNetRules();
}

int CGroup::getCumCPUUsage() {
  stringstream ss;
  ss<<BASE_URL<<CPUACCT_URL<<this->cgroup<<CPU_STAT_FILE;
  string file_path = ss.str();

  string content;
  Utils::readFile(file_path, content);

  // creating a copy of content array
  int length = content.length();
  char *ccontent = new char[length+1];
  memcpy(ccontent, content.c_str(), length);
  ccontent[length] = '\0';

  // parsing the cpuacct.stat file content
  strtok(ccontent, " ");
  int user_usage = atoi(strtok(NULL, "\n"));
  strtok(NULL, " ");
  int system_usage = atoi(strtok(NULL, "\n"));

  delete[] ccontent;
  return (system_usage+user_usage);
}

float CGroup::getCPUUsage() {
  int before_usage = this->getCumCPUUsage();
  usleep(USAGE_CHECK_PERIOD);
  int after_usage = this->getCumCPUUsage();
  return (float)((after_usage-before_usage)*(1000000/USAGE_CHECK_PERIOD));
}

float CGroup::getSoftCPUShares() {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_SHARES_FILE;
  string content;
  Utils::readFile(ss.str(), content);
  return (float)atoi(content.c_str())/1024*100;
}

float CGroup::getHardCPUShares() {
  float hard_shares;

  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_CFS_QUOTA_FILE;
  string content;
  Utils::readFile(ss.str(), content);
  int current_quota = atoi(content.c_str());

  if(current_quota == -1) {
    hard_shares = 100;
  } else {
    ss.str("");
    ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_CFS_PERIOD_FILE;
    content.clear();
    Utils::readFile(ss.str(), content);
    int period = atoi(content.c_str());
    hard_shares = (float)current_quota/((float)period)*100;
  }

  return hard_shares;
}

int CGroup::getPinnedCPUs() {
  string cpu_str;
  cpu_str.clear();

  stringstream ss;
  ss<<BASE_URL<<CPUSET_URL<<this->cgroup<<CPUSET_FILE;
  Utils::readFile(ss.str(), cpu_str);

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

void CGroup::setSoftCPUShares(float shares) {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_SHARES_FILE;

  stringstream content;
  int int_shares = (int)(shares/100*1024);
  content<<int_shares;
  Utils::writeFile(ss.str(), content.str());
}

void CGroup::setHardCPUShares(float shares) {
  if(shares > this->getPinnedCPUs()*100) {
    cout<<"Error: trying to allocate shares more than available!"<<endl;
    return;
  }

  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_CFS_PERIOD_FILE;

  string period_content;
  Utils::readFile(ss.str(), period_content);
  int period = atoi(period_content.c_str());

  ss.str("");
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_CFS_QUOTA_FILE;
  stringstream content;
  int quota = int(shares/100*int(period));
  content<<quota;
  Utils::writeFile(ss.str(), content.str());
}

float CGroup::getNetworkOutUsage() {
  stringstream ss;
  ss<<"cat /proc/"<<this->pid<<+"/net/dev | awk '/";
  ss<<this->interface<<"/ {print $"<<10<<"}'";

  string result;
  Utils::systemCmd(ss.str(), result);
  int before_usage = atoi(result.c_str());

  usleep(USAGE_CHECK_PERIOD);
  result.clear();
  Utils::systemCmd(ss.str(), result);
  int after_usage = atoi(result.c_str());
  return ((after_usage-before_usage)*(1000000/USAGE_CHECK_PERIOD))/1024;
}

float CGroup::getNetworkOutAllocation() {
  return bw;
}

void CGroup::setNetworkOutBW(float bw) {
  if(this->bw == -1) {
    this->bw = bw;
    this->last_handle += DEFAULT_HANDLE;

    stringstream ss;
    ss<<"tc class add dev "<<this->interface;
    ss<<" parent 1: classid 1:"<<this->last_handle<<" htb rate ";
    ss<<this->bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);

    ss.str("");
    ss<<"echo 0x1"<<setfill('0')<<setw(4)<<this->last_handle<<" > ";
    ss<<BASE_URL<<NET_CLASS<<"/"<<this->cgroup<<"/"<<CLASSID_FILE;
    Utils::systemCmd(ss.str(), 0);
  } else {
    this->bw = bw;
    stringstream ss;
    ss<<"tc class change dev "<<this->interface;
    ss<<" parent 1: classid 1:"<<this->last_handle<<" htb rate "<<bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);
  }
}
