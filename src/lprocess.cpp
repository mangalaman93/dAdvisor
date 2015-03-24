#include "lprocess.h"

LProcess::LProcess(string n, string iface, string filter, string cgroup)
    : Guest(n) {
  this->is_pid = false;
  this->is_cgroup = true;
  this->interface = iface;
  this->pid = 0;
  this->cgroup = cgroup;
  this->filter = filter;
  this->delNetRules();
  this->initNetRules();

  // other parameters
  this->in_bw = -1;
  this->out_bw = -1;
  this->shares = -1;
}

LProcess::LProcess(string n, string iface, string filter, int pid) : Guest(n) {
  this->is_pid = true;
  this->is_cgroup = false;
  this->interface = iface;
  this->pid = pid;
  this->cgroup = "";
  this->filter = filter;
  this->delNetRules();
  this->initNetRules();

  // other parameters
  this->in_bw = -1;
  this->out_bw = -1;
  this->shares = 100;
}

void LProcess::delNetRules() {
  // delete all existing rules on the given interface
  stringstream ss;
  ss<<"tc qdisc del dev "<<this->interface<<" root";
  Utils::systemCmd(ss.str());
}

void LProcess::initNetRules() {
  // init root handle for tc
  stringstream ss;
  ss<<"tc qdisc add dev "<<this->interface<<" root handle 1: htb default 30";
  Utils::systemCmd(ss.str(), 0);
}

LProcess::~LProcess() {
  delNetRules();
}

string LProcess::getType() const {
  return TYPE_LPROCESS;
}

// returns total CPU usage (user+system)
int LProcess::getCumCpuUsage() {
  stringstream ss;
  ss<<BASE_URL<<CPUACCT_URL<<cgroup<<CPU_STAT_FILE;
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

  return (system_usage+user_usage);
}

float LProcess::getCpuUsage() {
  float cpu_usage;

  if(is_pid) {
    stringstream ss;
    ss<<"top -b -w 60 -d 1 -p "<<this->pid<<" -n 2 | grep ";
    ss<<this->pid<<" | tail -n 1 | awk '{print $(NF-1)}'";
    string result;
    Utils::systemCmd(ss.str(), result);
    cpu_usage = atof(result.c_str());
  } else if(is_cgroup) {
    int before_usage = this->getCumCpuUsage();
    usleep(USAGE_CHECK_PERIOD);
    int after_usage = this->getCumCpuUsage();
    cpu_usage = (float)((after_usage-before_usage)*(1000000/USAGE_CHECK_PERIOD));
  }

  return cpu_usage;
}

float LProcess::getSoftCpuShares() {
  float soft_shares;

  if(is_pid) {
    soft_shares = this->shares;
  } else if(is_cgroup) {
    stringstream ss;
    ss<<BASE_URL<<CPU_URL<<cgroup<<CPU_SHARES_FILE;
    string content;
    Utils::readFile(ss.str(), content);
    soft_shares = atoi(content.c_str())/1024*100;
  }

  return soft_shares;
}

/* soft cpu shares is same as hard cpu shares when
   process id is given (i.e. is_pid == true) */
float LProcess::getHardCpuShares() {
  float hard_shares;

  if(is_pid) {
    hard_shares = shares;
  } else if(is_cgroup) {
    stringstream ss;
    ss<<BASE_URL<<CPU_URL<<cgroup<<CPU_CFS_QUOTA_FILE;
    string content;
    Utils::readFile(ss.str(), content);
    int current_quota = atoi(content.c_str());

    if(current_quota == -1) {
      hard_shares = 100;
    } else {
      ss.str("");
      ss<<BASE_URL<<CPU_URL<<cgroup<<CPU_CFS_PERIOD_FILE;
      content.clear();
      Utils::readFile(ss.str(), content);
      int period = atoi(content.c_str());
      hard_shares = int(current_quota/float(period)*100);
    }
  }

  return hard_shares;
}

int LProcess::getPinnedCpus() {
  string cpu_str;
  cpu_str.clear();

  if(is_pid) {
    stringstream ss;
    ss<<"taskset -c -p "<<this->pid;
    string result;
    Utils::systemCmd(ss.str(), result);

    int index;
    for(int i=0; i<result.length(); i++) {
      if(result[i] == ':') {
        index = i;
        break;
      }
    }

    index += 2;
    for(int i=index; i<result.length(); i++) {
      cpu_str.push_back(result[i]);
    }
  } else if(is_cgroup) {
    stringstream ss;
    ss<<BASE_URL<<CPUSET_URL<<cgroup<<CPUSET_FILE;
    Utils::readFile(ss.str(), cpu_str);
  }

  bool flag = false;
  int total_cpus = 0;
  for(int i=0; i<cpu_str.length(); i++) {
    while(true) {
      char num[5];
      int j=0;

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

int LProcess::getCumNetworkUsage(string file) {
  string result;
  stringstream ss;
  ss<<"cat "+file;
  Utils::systemCmd(ss.str(), result);

  return atoi(result.c_str());
}

// in kbytes/s
float LProcess::getNetworkUsageHelper(string file) {
  int before_usage = this->getCumNetworkUsage(file);
  usleep(USAGE_CHECK_PERIOD);
  int after_usage = this->getCumNetworkUsage(file);
  return ((after_usage-before_usage)*(1000000/USAGE_CHECK_PERIOD))/1024;
}

float LProcess::getNetworkInUsage() {
  stringstream ss;
  ss<<NET_FILE_PREFIX<<interface<<"/statistics/rx_bytes";
  return this->getNetworkUsageHelper(ss.str());
}

float LProcess::getNetworkOutUsage() {
  stringstream ss;
  ss<<NET_FILE_PREFIX<<interface<<"/statistics/tx_bytes";
  return this->getNetworkUsageHelper(ss.str());
}

float LProcess::getNetworkInAllocation() {
  return in_bw;
}

float LProcess::getNetworkOutAllocation() {
  return out_bw;
}

void LProcess::setSoftCpuShares(float shares) {
  if(is_pid) {
    this->setHardCpuShares(shares);
  } else if(is_cgroup) {
    stringstream ss;
    ss<<BASE_URL<<CPU_URL<<cgroup<<CPU_SHARES_FILE;

    stringstream content;
    int int_shares = (int)(shares/100*1024);
    content<<int_shares;
    Utils::writeFile(ss.str(), content.str());
  }
}

/* ref: http://blog.scoutapp.com/articles/2014/11/04/ */
void LProcess::setHardCpuShares(float shares) {
  if(shares > this->getPinnedCpus()*100) {
    cout<<"trying to allocate shares more than available!"<<endl;
    SHOULD_NOT_REACH();
  }

  if(is_pid) {
    this->shares = shares;
    Utils::systemCmd("sudo killall -9 cpulimit");

    int limit = (int)shares;
    stringstream ss;
    ss<<"sudo cpulimit -bz -p "<<this->pid<<" -l "<<limit;
    Utils::systemCmd(ss.str());
  } else if(is_cgroup) {
    stringstream ss;
    ss<<BASE_URL<<CPU_URL<<cgroup<<CPU_CFS_PERIOD_FILE;

    string period_content;
    Utils::readFile(ss.str(), period_content);
    int period = atoi(period_content.c_str());

    ss.str("");
    ss<<BASE_URL<<CPU_URL<<cgroup<<CPU_CFS_QUOTA_FILE;
    stringstream content;
    int quota = int(shares/100*int(period));
    content<<quota;
    Utils::writeFile(ss.str(), content.str());
  }
}

void LProcess::setNetworkInBW(float bw) {
  if(this->in_bw == -1) {
    in_bw = bw;
    stringstream ss;
    ss<<"tc class add dev "<<this->interface;
    ss<<" parent 1: classid 1:1 htb rate ";
    ss<<in_bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);

    ss.str("");
    string ip = Utils::getIPAddr(this->interface);

    ss<<"tc filter add dev "<<this->interface;
    ss<<" protocol ip parent 1:0 prio 1 u32 match ip dst "<<ip.c_str();
    ss<<"/32 "<<this->filter<<" flowid 1:1";
    Utils::systemCmd(ss.str(), 0);
  } else {
    in_bw = bw;
    stringstream ss;
    ss<<"tc class change dev "<<this->interface;
    ss<<" parent 1: classid 1:1 htb rate "<<in_bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);
  }
}

void LProcess::setNetworkOutBW(float bw) {
  if(this->out_bw == -1) {
    out_bw = bw;
    stringstream ss;
    ss<<"tc class add dev "<<this->interface;
    ss<<" parent 1: classid 1:2 htb rate ";
    ss<<out_bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);

    ss.str("");
    string ip = Utils::getIPAddr(this->interface);

    ss<<"tc filter add dev "<<this->interface;
    ss<<" protocol ip parent 1:0 prio 1 u32 match ip dst "<<ip.c_str();
    ss<<"/32 "<<this->filter<<" flowid 1:2";
    Utils::systemCmd(ss.str(), 0);
  } else {
    out_bw = bw;
    stringstream ss;
    ss<<"tc class change dev "<<this->interface;
    ss<<" parent 1: classid 1:2 htb rate "<<out_bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);
  }
}
