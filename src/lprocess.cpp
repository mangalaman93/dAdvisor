#include "lprocess.h"

LProcess::LProcess(string n, string cgroup) : Guest(n) {
  init(DEFAULT_INTERFACE, cgroup);
}

LProcess::LProcess(string n, string interface, string cgroup) : Guest(n) {
  init(interface, cgroup);
}

void LProcess::init(string interface, string cgroup) {
  stringstream ss;
  Utils::systemCmd(ss.str());
}

LProcess::LProcess(string n, int pid) : Guest(n) {
  init(DEFAULT_INTERFACE, pid);
}

LProcess::LProcess(string n, string interface, int pid) : Guest(n) {
  init(interface, pid);
}

void LProcess::init(string interface, int pid) {
}

LProcess::~LProcess() {
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
}

void LProcess::setNetworkOutBW(float bw) {
}
