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
  this->out_bw = -1;

  // get the pid of the process running for network metrics
  stringstream ss;
  ss<<"cat "<<BASE_URL<<NET_CLASS<<this->cgroup<<TASKS_FILE;
  ss<<" | head -1";
  string result;
  Utils::systemCmd(ss.str(), result);
  this->pid = atoi(result.c_str());
}

CGroup::~CGroup() {
  this->delNetRules();
}

string CGroup::getType() const {
  return TYPE_CGROUP;
}

unsigned long CGroup::getCPUCumUsage() {
  stringstream ss;
  ss<<BASE_URL<<CPUACCT_URL<<this->cgroup<<CPU_STAT_FILE;
  string file_path = ss.str();

  string content;
  Utils::readFile(file_path, content);

  int user_usage, system_usage;
  sscanf(content.c_str(), "user %dsystem %d", &user_usage, &system_usage);
  return (system_usage+user_usage);
}

float CGroup::getSoftCPUShares() {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_SHARES_FILE;
  string content;
  Utils::readFile(ss.str(), content);
  return (float)atoi(content.c_str())*100.0/1024.0;
}

float CGroup::getHardCPUShares() {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_CFS_QUOTA_FILE;
  string content;
  Utils::readFile(ss.str(), content);
  int current_quota = atoi(content.c_str());

  if(current_quota == -1) {
    return 100.0;
  } else {
    ss.str("");
    ss<<BASE_URL<<CPU_URL<<this->cgroup<<CPU_CFS_PERIOD_FILE;
    content.clear();
    Utils::readFile(ss.str(), content);
    int period = atoi(content.c_str());
    return current_quota/((float)period)*100.0;
  }
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
  int int_shares = (int)(shares*1024/100);
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

unsigned long CGroup::getNetworkOutCumUsage() {
  stringstream ss;
  ss<<"cat /proc/"<<this->pid<<+"/net/dev | awk '/";
  ss<<this->interface<<"/ {print $"<<10<<"}'";

  string result;
  Utils::systemCmd(ss.str(), result);
  return atoi(result.c_str());
}

float CGroup::getNetworkOutBW() {
  return out_bw;
}

void CGroup::setNetworkOutBW(float out_bw) {
  if(this->out_bw == -1) {
    this->out_bw = out_bw;
    this->last_handle += DEFAULT_HANDLE;

    string bw = Utils::getStringBW(out_bw);
    stringstream ss;
    ss<<"tc class add dev "<<this->interface;
    ss<<" parent 1: classid 1:"<<this->last_handle<<" htb rate "<<bw;
    Utils::systemCmd(ss.str(), 0);

    ss.str("");
    ss<<"tc filter add dev "<<this->interface;
    ss<<" parent 1: protocol ip prio 10 handle ";
    ss<<this->last_handle<<": cgroup";
    Utils::systemCmd(ss.str(), 0);

    ss.str("");
    ss<<"echo 0x1"<<setfill('0')<<setw(4)<<this->last_handle<<" > ";
    ss<<BASE_URL<<NET_CLASS<<this->cgroup<<CLASSID_FILE;
    Utils::systemCmd(ss.str(), 0);
  } else {
    this->out_bw = out_bw;
    string bw = Utils::getStringBW(out_bw);

    stringstream ss;
    ss<<"tc class change dev "<<this->interface;
    ss<<" parent 1: classid 1:"<<this->last_handle<<" htb rate "<<bw;
    Utils::systemCmd(ss.str(), 0);
  }
}
