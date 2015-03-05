#include "lprocess.h"

LProcess::LProcess(string n) : Guest(n) {
}

LProcess::~LProcess() {
}

string LProcess::getType() const {
  return TYPE_LPROCESS;
}

float LProcess::get_cpu_usage() {
}

float LProcess::get_soft_cpu_shares() {
}

float LProcess::get_hard_cpu_shares() {
}

int LProcess::get_pinned_cpus() {
}

float LProcess::get_network_in_usage() {
}

float LProcess::get_network_out_usage() {
}

float LProcess::get_network_in_allocation() {
}

float LProcess::get_network_out_allocation() {
}

void LProcess::set_soft_cpu_shares(float shares) {
}

void LProcess::set_hard_cpu_shares(float shares) {
}

void LProcess::set_network_in_bw(float bw) {
}

void LProcess::set_network_out_bw(float bw) {
}
