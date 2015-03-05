#include "container.h"

Container::Container(string n) : Guest(n) {
}

Container::~Container() {
}

string Container::getType() const {
  return TYPE_CONTAINER;
}

float Container::get_cpu_usage() {
}

float Container::get_soft_cpu_shares() {
}

float Container::get_hard_cpu_shares() {
}

int Container::get_pinned_cpus() {
}

float Container::get_network_in_usage() {
}

float Container::get_network_out_usage() {
}

float Container::get_network_in_allocation() {
}

float Container::get_network_out_allocation() {
}

void Container::set_soft_cpu_shares(float shares) {
}

void Container::set_hard_cpu_shares(float shares) {
}

void Container::set_network_in_bw(float bw) {
}

void Container::set_network_out_bw(float bw) {
}
