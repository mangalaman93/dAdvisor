#include "guest.h"

Guest::Guest(string n) {
  name = n;
}

Guest::~Guest() {}

string Guest::getName() const {
  return name;
}

bool Guest::equals(const Guest& guest) const {
  return (guest.getType().compare(this->getType()) &&
          this->getName().compare(guest.getName()));
}

float Guest::getCpuUsage() {
  SHOULD_NOT_REACH();
}

float Guest::getSoftCpuShares() {
  SHOULD_NOT_REACH();
}

float Guest::getHardCpuShares() {
  SHOULD_NOT_REACH();
}

int Guest::getPinnedCpus() {
  SHOULD_NOT_REACH();
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

void Guest::setSoftCpuShares(float shares) {
  SHOULD_NOT_REACH();
}

void Guest::setHardCpuShares(float shares) {
  SHOULD_NOT_REACH();
}

void Guest::setNetworkInBW(float bw) {
  SHOULD_NOT_REACH();
}

void Guest::setNetworkOutBW(float bw) {
  SHOULD_NOT_REACH();
}
