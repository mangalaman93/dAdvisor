#include "lprocess.h"

LProcess::LProcess(string n, string filter, string cgroup) : Guest(n) {
  init(DEFAULT_INTERFACE, filter, cgroup);
}

LProcess::LProcess(string n, string iface, string filter, string cgroup) : Guest(n) {
  init(iface, filter, cgroup);
}

LProcess::LProcess(string n, string filter, int pid) : Guest(n) {
  init(DEFAULT_INTERFACE, filter, pid);
}

LProcess::LProcess(string n, string iface, string filter, int pid) : Guest(n) {
  init(iface, filter, pid);
}

void LProcess::init(string iface, string filter, string cgroup) {
  this->is_pid = false;
  this->is_cgroup = true;
  this->interface = iface;
  this->pid = 0;
  this->cgroup = cgroup;
  this->filter = filter;
  this->delNetRules();
  this->initNetRules();

  // other parameters
  in_bw = 0;
  out_bw = 0;
}

void LProcess::init(string iface, string filter, int pid) {
  this->is_pid = true;
  this->is_cgroup = false;
  this->interface = iface;
  this->pid = pid;
  this->cgroup = "";
  this->filter = filter;
  this->delNetRules();
  this->initNetRules();

  // other parameters
  in_bw = 0;
  out_bw = 0;
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

float LProcess::getCpuUsage() {
}

float LProcess::getSoftCpuShares() {
}

float LProcess::getHardCpuShares() {
}

int LProcess::getPinnedCpus() {
}

float LProcess::getNetworkInUsage() {
}

float LProcess::getNetworkOutUsage() {
}

float LProcess::getNetworkInAllocation() {
}

float LProcess::getNetworkOutAllocation() {
}

void LProcess::setSoftCpuShares(float shares) {
}

void LProcess::setHardCpuShares(float shares) {
}

void LProcess::setNetworkInBW(float bw) {
  stringstream ss;

  if(this->in_bw == 0) {
    in_bw = bw;
    ss<<"tc class add dev "<<this->interface;
    ss<<" parent 1: classid 1:1 htb rate ";
    ss<<in_bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);

    ss.str("");
    string ip = Utils::getIPAddr(this->interface);

    ss<<"tc filter add dev "<<this->interface;
    ss<<" protocol ip parent 1:0 prio 1 u32 match ip dst "<<ip.c_str();
    ss<<"/32 flowid 1:1";
    Utils::systemCmd(ss.str(), 0);
  } else {
    in_bw = bw;
    ss<<"tc class change dev "<<this->interface;
    ss<<" parent 1: classid 1:1 htb rate "<<in_bw<<"kbps";
    Utils::systemCmd(ss.str(), 0);
  }
}

void LProcess::setNetworkOutBW(float bw) {
}
