#include <iostream>
#include <mutex>
#include <vector>
#include <unistd.h>
#include "config.h"
#include "guest.h"
using namespace std;

class Monitor {
  vector<Guest*> gobjs;
  bool is_done;
  mutex mtx;

protected:
  virtual void checkAndSet(Guest *guest) = 0;

public:
  Monitor();
  ~Monitor();
  void addGuest(Guest *guest);
  void run();
  void stop();
};
