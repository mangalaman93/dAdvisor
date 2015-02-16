# monitoring logic
# triggers periodic check and implements the
# dynamic allocation model

import core.container
import threading
import time

# parameters
TRIGGER_PERIOD = 4         # period of monitoring in sec
SAFETY_FACTOR = 0.8        # allocation safety factor
INCREMENT_FACTOR = 0.2     # increase allocation by 0.2

class Monitor(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    self.done = False
    self.containers = []
    self.lock = threading.Lock()

  # stops the thread
  def stop(self):
    self.done = True

  # add a container object to the list
  def add_container(self, container):
    with self.lock:
      if self.containers.count(container) == 1:
        print "**Error: container ",container.name," already exists"
      else:
        self.containers.append(container)

  # remove a container object from the list
  def rm_container(self, container):
    with self.lock:
      if self.containers.count(container) == 1:
        self.containers.remove(container)
      else:
        print "**Error: container",container.name,"doesn't exist"

  def run(self):
    while not self.done:
      time.sleep(TRIGGER_PERIOD)
      with self.lock:
        if not self.containers:
          continue
        for container in self.containers:
          # network allocation
          usage = container.get_network_in_usage()
          allocation = container.get_network_in_allocation()
          print "usage {0}, allocation {1} for container {2}"\
                .format(usage, allocation, container.name)
          if(allocation*SAFETY_FACTOR < usage):
            print "usage {0} crossed allocation {1} for container {2}"\
                  .format(usage, allocation, container.name)
            container.set_network_in_bw(allocation*(1+INCREMENT_FACTOR))
