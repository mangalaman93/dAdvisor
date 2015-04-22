#include "monitor.h"

Monitor::Monitor() {
  is_done = false;
}

Monitor::~Monitor() {}

void Monitor::addGuest(Guest *guest) {
  mtx.lock();
  gobjs.push_back(guest);
  mtx.unlock();
}

void Monitor::run() {
  while(!this->is_done) {
    sleep(USAGE_CHECK_PERIOD);

    if(gobjs.size() <= 0) {
      continue;
    }

    // acquiring lock
    mtx.lock();

    vector<Guest*>::iterator iter;
    for(iter=gobjs.begin(); iter!=gobjs.end(); ++iter) {
      this->checkAndSet(*iter);
    }

    // releasing lock
    mtx.unlock();
  }
}

void Monitor::stop() {
  mtx.lock();
  is_done = true;
  mtx.unlock();
}
