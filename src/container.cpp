#include "container.h"

void Container::initNetRules() {
  stringstream ss;
  ss<<"nsenter -t $(docker inspect --format '{{ .State.Pid }}' ";
  ss<<this->id<<") -n tc qdisc add dev "<<DEFAULT_DOCKER_IFACE;
  ss<<" root handle 1: htb default 10";
  Utils::systemCmd(ss.str(), 0);
}

void Container::delNetRules() {
  stringstream ss;
  ss<<"nsenter -t $(docker inspect --format '{{ .State.Pid }}' ";
  ss<<this->id<<") -n tc qdisc del dev "<<DEFAULT_DOCKER_IFACE<<" root";
  Utils::systemCmd(ss.str());
}

Container::Container(string n, string id) : Guest(n) {
  stringstream ss;
  ss<<"docker inspect --format '{{ .Id }}' "<<id;
  Utils::systemCmd(ss.str(), this->id);
  this->delNetRules();
  this->initNetRules();

  // other settings
  this->out_bw = -1;
}

Container::~Container() {
  this->delNetRules();
}

string Container::getType() const {
  return TYPE_CONTAINER;
}

unsigned long Container::getCPUCumUsage() {
  stringstream ss;
  ss<<BASE_URL<<CPUACCT_URL<<DOCKER_URL<<this->id<<"/"<<CPU_STAT_FILE;
  string file_path = ss.str();

  string content;
  Utils::readFile(file_path, content);

  int user_usage, system_usage;
  sscanf(content.c_str(), "user %dsystem %d", &user_usage, &system_usage);
  return (system_usage+user_usage);
}

float Container::getSoftCPUShares() {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<DOCKER_URL<<this->id<<"/"<<CPU_SHARES_FILE;
  string content;
  Utils::readFile(ss.str(), content);
  return (float)atoi(content.c_str())*100.0/1024.0;
}

float Container::getHardCPUShares() {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<DOCKER_URL<<this->id<<"/"<<CPU_CFS_QUOTA_FILE;
  string content;
  Utils::readFile(ss.str(), content);
  int current_quota = atoi(content.c_str());

  if(current_quota == -1) {
    return 1024;
  } else {
    ss.str("");
    ss<<BASE_URL<<CPU_URL<<DOCKER_URL<<this->id<<"/"<<CPU_CFS_PERIOD_FILE;
    content.clear();
    Utils::readFile(ss.str(), content);
    int period = atoi(content.c_str());
    return (unsigned int)current_quota/((float)period)*100.0;
  }
}

int Container::getPinnedCPUs() {
  string cpu_str;
  cpu_str.clear();

  stringstream ss;
  ss<<BASE_URL<<CPUSET_URL<<DOCKER_URL<<this->id<<"/"<<CPUSET_FILE;
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

void Container::setSoftCPUShares(float shares) {
  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<DOCKER_URL<<this->id<<"/"<<CPU_SHARES_FILE;

  stringstream content;
  int int_shares = (int)(shares*1024.0/100.0);
  content<<int_shares;
  Utils::writeFile(ss.str(), content.str());
}

void Container::setHardCPUShares(float shares) {
  if(shares > this->getPinnedCPUs()*100.0) {
    cout<<"Error: trying to allocate shares more than available!"<<endl;
    return;
  }

  stringstream ss;
  ss<<BASE_URL<<CPU_URL<<DOCKER_URL<<this->id<<"/"<<CPU_CFS_PERIOD_FILE;

  string period_content;
  Utils::readFile(ss.str(), period_content);
  int period = atoi(period_content.c_str());

  ss.str("");
  ss<<BASE_URL<<CPU_URL<<DOCKER_URL<<this->id<<"/"<<CPU_CFS_QUOTA_FILE;
  stringstream content;
  int quota = int(shares/100*int(period));
  content<<quota;
  Utils::writeFile(ss.str(), content.str());
}

unsigned long Container::getNetworkOutCumUsage() {
  stringstream ss;
  ss<<"docker exec "<<this->id<<" cat "<<NET_TX_FILE;

  string result;
  Utils::systemCmd(ss.str(), result);
  return atoi(result.c_str());
}

float Container::getNetworkOutBW() {
  return out_bw;
}

void Container::setNetworkOutBW(float out_bw) {
  if(this->out_bw == -1) {
    this->out_bw = out_bw;
    string bw = Utils::getStringBW(out_bw);

    stringstream ss;
    ss<<"nsenter -t $(docker inspect --format '{{ .State.Pid }}' "<<this->id;
    ss<<") -n tc class add dev "<<DEFAULT_DOCKER_IFACE<<" parent 1: classid";
    ss<<" 1:10 htb rate "<<bw;
    Utils::systemCmd(ss.str(), 0);
  } else {
    this->out_bw = out_bw;
    string bw = Utils::getStringBW(out_bw);

    stringstream ss;
    ss<<"nsenter -t $(docker inspect --format '{{ .State.Pid }}' "<<this->id;
    ss<<") -n tc class change dev "<<DEFAULT_DOCKER_IFACE<<" parent 1: classid";
    ss<<" 1:10 htb rate "<<bw;
    Utils::systemCmd(ss.str(), 0);
  }
}
