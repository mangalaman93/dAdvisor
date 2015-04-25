#include <iostream>
#include <unistd.h>
#include "config.h"
#include "guest.h"
using namespace std;

class Monitor {
  bool done;

protected:
  Guest* gobj;
  virtual void checkAndSet() = 0;

public:
  Monitor(Guest *guest);
  ~Monitor();
  void run();
  void stop();
};
