# monitoring logic
# triggers periodic check and implements the
# dynamic allocation model

import container
import threading
import time

# parameters
TRIGGER_PERIOD = 4         # period of monitoring in sec

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
        print "**Error: container",container.name,"already exists"
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
        # @todo implement main logic here
