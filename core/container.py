# container class
# allows to get usage statistics for a docker container
# and change resource allocation for the container
# needs root permission while using the class

import os
import subprocess
import sys
import time

# parameters
USAGE_CHECK_PERIOD = 0.5   # period before collect usage again

# constants
LEN_ID = 64

# CGROUP constants
BASE_URL = "/sys/fs/cgroup/"
CPUACCT_URL = "cpuacct/"
CPU_URL = "cpu/"
CPUSET_URL = "cpuset/"
DOCKER_URL = "docker/"
CPU_STAT_FILE = "cpuacct.stat"
CPU_SHARES_FILE = "cpu.shares"
CPUSET_FILE = "cpuset.cpus"
CPU_CFS_QUOTA_FILE = "cpu.cfs_quota_us"
CPU_CFS_PERIOD_FILE = "cpu.cfs_period_us"

class Container(object):
  """docker container class"""

  # constructor
  def __init__(self, id):
    super(Container, self).__init__()
    # if name is provided instead of container id
    if len(id) == LEN_ID:
      self.name = None
      self.id = id
    else:
      p = subprocess.Popen('docker inspect '+id+' | grep Id',
                           shell=True, stdout=subprocess.PIPE)
      # if only the command executed successfully
      if p.wait() == 0:
        self.name = id
        self.id = p.stdout.readlines()[0].split("\"")[3]
      else:
        raise Exception("invalid container id")

  # check whether a directory exists
  def exists(self, cdir):
    if not os.path.isdir(cdir):
      raise Exception("container doesn't exist!")

  # general read file function
  def read_file(self, filePath):
    f = open(filePath, 'r')
    data = f.read()
    f.close()
    return data

  # returns total CPU usage (user+system)
  def get_cum_cpu_usage(self, filePath):
    content = self.read_file(filePath)
    content_list = content.split()
    # adding system and user usage
    return (int(content_list[1]) + int(content_list[3]))

  # get cpu usage by the container
  def get_cpu_usage(self):
    cdir = os.path.join(BASE_URL, CPUACCT_URL, DOCKER_URL, self.id)
    self.exists(cdir)
    filePath = os.path.join(cdir, CPU_STAT_FILE)
    before_usage = self.get_cum_cpu_usage(filePath)
    time.sleep(USAGE_CHECK_PERIOD)
    after_usage = self.get_cum_cpu_usage(filePath)
    return (after_usage-before_usage)/USAGE_CHECK_PERIOD

  # get current allocated CPU shares
  def get_soft_cpu_shares(self):
    cdir = os.path.join(BASE_URL, CPU_URL, DOCKER_URL, self.id)
    self.exists(cdir)
    return int(self.read_file(os.path.join(cdir, CPU_SHARES_FILE)))

  # get hard limit on cpu shares
  def get_hard_cpu_shares(self):
    cdir = os.path.join(BASE_URL, CPU_URL, DOCKER_URL, self.id)
    current_quota = int(self.read_file(os.path.join(cdir, CPU_CFS_QUOTA_FILE)))
    if current_quota == -1:
      return 1024
    else:
      period = self.read_file(os.path.join(cdir, CPU_CFS_PERIOD_FILE))
      return int(current_quota/float(period)*1024)

  # get maximum number of CPUs allowed to be used (CPU pinning)
  def get_pinned_cpus(self):
    cdir = os.path.join(BASE_URL, CPUSET_URL, DOCKER_URL, self.id)
    cpu_str = self.read_file(os.path.join(cdir, CPUSET_FILE))
    total_cpus = 0
    for cpus in cpu_str.split(','):
      cpu_range = cpus.split('-')
      if len(cpu_range) == 1:
        total_cpus = total_cpus+1
      else:
        total_cpus = total_cpus + 1 + int(cpu_range[1]) - int(cpu_range[0])
    return total_cpus
