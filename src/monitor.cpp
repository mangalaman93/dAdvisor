#include "monitor.h"

Monitor::Monitor(Guest* guest) {
  done = false;
  gobj = guest;
}

Monitor::~Monitor() {}

void Monitor::run() {
  while(!this->done) {
    sleep(USAGE_CHECK_PERIOD);
    this->checkAndSet();
  }
}

void Monitor::stop() {
  done = true;
}
