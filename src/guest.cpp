#include "guest.h"

Guest::Guest(string n) {
  this->name = n;
}

Guest::~Guest() {}

string Guest::getName() const {
  return this->name;
}

bool Guest::equals(const Guest& guest) const {
  return (guest.getType().compare(this->getType()) &&
          guest.getName().compare(this->getName()));
}

float Guest::getNetworkInUsage() {
  SHOULD_NOT_REACH();
}

float Guest::getNetworkOutUsage() {
  SHOULD_NOT_REACH();
}

float Guest::getNetworkInAllocation() {
  SHOULD_NOT_REACH();
}

float Guest::getNetworkOutAllocation() {
  SHOULD_NOT_REACH();
}

void Guest::setNetworkInBW(float bw) {
  SHOULD_NOT_REACH();
}

void Guest::setNetworkOutBW(float bw) {
  SHOULD_NOT_REACH();
}

float Guest::getCPUUsage() {
  SHOULD_NOT_REACH();
}

float Guest::getSoftCPUShares() {
  SHOULD_NOT_REACH();
}

float Guest::getHardCPUShares() {
  SHOULD_NOT_REACH();
}

int Guest::getPinnedCPUs() {
  SHOULD_NOT_REACH();
}

void Guest::setSoftCPUShares(float shares) {
  SHOULD_NOT_REACH();
}

void Guest::setHardCPUShares(float shares) {
  SHOULD_NOT_REACH();
}

void Guest::setPinnedCPUs(int num_cpus) {
  SHOULD_NOT_REACH();
}
