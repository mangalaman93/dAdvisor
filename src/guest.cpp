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

unsigned long Guest::getNetworkInCumUsage() {
  SHOULD_NOT_REACH();
}

unsigned long Guest::getNetworkOutCumUsage() {
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

unsigned long Guest::getCPUCumUsage() {
  SHOULD_NOT_REACH();
}

unsigned int Guest::getSoftCPUShares() {
  SHOULD_NOT_REACH();
}

unsigned int Guest::getHardCPUShares() {
  SHOULD_NOT_REACH();
}

unsigned int Guest::getPinnedCPUs() {
  SHOULD_NOT_REACH();
}

void Guest::setSoftCPUShares(unsigned int shares) {
  SHOULD_NOT_REACH();
}

void Guest::setHardCPUShares(unsigned int shares) {
  SHOULD_NOT_REACH();
}

void Guest::setPinnedCPUs(unsigned int num_cpus) {
  SHOULD_NOT_REACH();
}
