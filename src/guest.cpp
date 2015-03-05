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

void Guest::systemCmd(string cmd, int ret) {
  if(ret != system(cmd.c_str())) {
    cout<<"unsuccessful to run command: "<<cmd<<" ";
    LOG_POS();
    assert(0);
  }
}

float Guest::get_cpu_usage() {
  SHOULD_NOT_REACH();
}

float Guest::get_soft_cpu_shares() {
  SHOULD_NOT_REACH();
}

float Guest::get_hard_cpu_shares() {
  SHOULD_NOT_REACH();
}

int Guest::get_pinned_cpus() {
  SHOULD_NOT_REACH();
}

float Guest::get_network_in_usage() {
  SHOULD_NOT_REACH();
}

float Guest::get_network_out_usage() {
  SHOULD_NOT_REACH();
}

float Guest::get_network_in_allocation() {
  SHOULD_NOT_REACH();
}

float Guest::get_network_out_allocation() {
  SHOULD_NOT_REACH();
}

void Guest::set_soft_cpu_shares(float shares) {
  SHOULD_NOT_REACH();
}

void Guest::set_hard_cpu_shares(float shares) {
  SHOULD_NOT_REACH();
}

void Guest::set_network_in_bw(float bw) {
  SHOULD_NOT_REACH();
}

void Guest::set_network_out_bw(float bw) {
  SHOULD_NOT_REACH();
}
