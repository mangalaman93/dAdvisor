#include "container.h"

Container::Container(string n) : Guest(n) {
}

Container::~Container() {
}

string Container::getType() const {
  return TYPE_CONTAINER;
}

float Container::getCpuUsage() {
}

float Container::getSoftCpuShares() {
}

float Container::getHardCpuShares() {
}

int Container::getPinnedCpus() {
}

float Container::getNetworkInUsage() {
}

float Container::getNetworkOutUsage() {
}

float Container::getNetworkInAllocation() {
}

float Container::getNetworkOutAllocation() {
}

void Container::setSoftCpuShares(float shares) {
}

void Container::setHardCpuShares(float shares) {
}

void Container::setNetworkInBW(float bw) {
}

void Container::setNetworkOutBW(float bw) {
}
