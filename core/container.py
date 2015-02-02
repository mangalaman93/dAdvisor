# container class
# allows to get usage statistics for a docker container
# and change resource allocation for the container
# needs root permission while using the class
# requires docker 1.3 to get network statistics
# requires tc,cat tools to be installed in the container

import time
from guest import *

# parameters
USAGE_CHECK_PERIOD = 1   # period before collect usage again

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
NET_TX_FILE = "/sys/devices/virtual/net/eth0/statistics/tx_bytes"
NET_RX_FILE = "/sys/devices/virtual/net/eth0/statistics/rx_bytes"

# Network constants
INT_INTERFACE = "eth0"
ROOT_HANDLE = "1:"
CLASS_HANDLE = "30"

class Container(Guest):
  """docker container class"""

  # constructor
  def __init__(self, id):
    super(Container, self).__init__(id)
    # if name is provided instead of container id
    if len(id) == LEN_ID:
      self.id = id
    else:
      cmd = 'docker inspect '+id+' | grep Id'
      self.id = self.system_cmd(cmd, "invalid container id")[0].split("\"")[3]

  # general read file function
  def read_file(self, filePath):
    f = open(filePath, 'r')
    data = f.read()
    f.close()
    return data

  # writes into a file
  def write_file(self, filePath, data):
    f = open(filePath, 'w')
    f.write(str(data)+"\n")
    f.close()

  # returns total CPU usage (user+system)
  def get_cum_cpu_usage(self, filePath):
    content = self.read_file(filePath)
    content_list = content.split()
    # adding system and user usage
    return (int(content_list[1]) + int(content_list[3]))

  # total consumed network till now
  def get_cum_network_usage(self, File):
    cmd = 'docker exec '+self.id+' cat '+File
    return int(self.system_cmd(cmd, "")[0])

  # helper function
  def get_network_usage_helper(self, File):
    before_usage = self.get_cum_network_usage(File)
    time.sleep(USAGE_CHECK_PERIOD)
    after_usage = self.get_cum_network_usage(File)
    return (after_usage-before_usage)/USAGE_CHECK_PERIOD

  # get cpu usage by the container
  def get_cpu_usage(self):
    cdir = os.path.join(BASE_URL, CPUACCT_URL, DOCKER_URL, self.id)
    self.exists(cdir)
    filePath = os.path.join(cdir, CPU_STAT_FILE)
    before_usage = self.get_cum_cpu_usage(filePath)
    time.sleep(USAGE_CHECK_PERIOD)
    after_usage = self.get_cum_cpu_usage(filePath)
    return (after_usage-before_usage)/USAGE_CHECK_PERIOD*1024/100.0

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

  # received network traffic rate (kbytes per sec)
  def get_network_in_usage(self):
    return self.get_network_usage_helper(NET_RX_FILE)/1000.0

  # sent network traffic rate (kbytes per sec)
  def get_network_out_usage(self):
    return self.get_network_usage_helper(NET_TX_FILE)/1000.0

  # set given value of cpu shares for the container
  def set_soft_cpu_shares(self, shares):
    cdir = os.path.join(BASE_URL, CPU_URL, DOCKER_URL, self.id)
    self.exists(cdir)
    self.write_file(os.path.join(cdir, CPU_SHARES_FILE), shares)

  # set allocated CPU to the container (shares=1024=>1core)
  def set_hard_cpu_shares(self, shares):
    cdir = os.path.join(BASE_URL, CPU_URL, DOCKER_URL, self.id)
    period = self.read_file(os.path.join(cdir, CPU_CFS_PERIOD_FILE))
    if shares > self.get_pinned_cpus()*1024:
      raise Exception("trying to allocate shares more than available")
    self.write_file(os.path.join(cdir, CPU_CFS_QUOTA_FILE),
                    int(shares/1024.0*int(period)))

  # set network-in bandwidth, bw in kbps (kbytes per sec)
  def set_network_in_bw(self, bw):
    if self.in_bw:
      self.in_bw = bw
      cmd = "tc class change dev "+self.ext_iface+" parent "+self.root_handle\
            +" classid "+self.root_handle+self.class_handle+" htb rate "\
            +str(self.in_bw)+"kbps"
      self.system_cmd(cmd, "unable to execute tc command on host")
    else:
      self.in_bw = bw
      cmd = "ip link list | grep $(nsenter -t $(docker inspect --format "\
            +"'{{ .State.Pid }}' "+self.id+") -n ethtool -S "+INT_INTERFACE\
            +" | grep peer | awk '{print $2}') | awk '{print $2}' | cut -d "\
            +"':' -f 1 | tr -d '\n'"
      self.ext_iface = self.system_cmd(cmd, "unable to find peer interface")[0]
      self.root_handle = ROOT_HANDLE
      self.class_handle = CLASS_HANDLE
      cmd = "tc qdisc add dev "+self.ext_iface+" root handle "\
            +self.root_handle+" htb default "+self.class_handle
      self.system_cmd(cmd, "unable to execute tc command1 on host")
      cmd = "tc class add dev "+self.ext_iface+" parent "+self.root_handle\
            +" classid "+self.root_handle+self.class_handle+" htb rate "\
            +str(self.in_bw)+"kbps"
      self.system_cmd(cmd, "unable to execute tc command2 on host")

  # set network-out bandwidth, bw in kbytes/sec
  def set_network_out_bw(self, bw):
    if self.out_bw:
      self.out_bw = bw
      cmd = "nsenter -t $(docker inspect --format '{{ .State.Pid }}' "+self.id\
            +") -n tc class change dev "+self.iface+" parent "+self.root_handle\
            +" classid "+self.root_handle+self.class_handle+" htb rate "\
            +str(self.out_bw)+"kbps"
      self.system_cmd(cmd, "unable to execute tc command on host")
    else:
      self.out_bw = bw
      self.iface = INT_INTERFACE
      self.root_handle = ROOT_HANDLE
      self.class_handle = CLASS_HANDLE
      cmd = "nsenter -t $(docker inspect --format '{{ .State.Pid }}' "+self.id\
            +") -n tc qdisc add dev "+self.iface+" root handle "\
            +self.root_handle+" htb default "+self.class_handle
      self.system_cmd(cmd, "unable to execute tc command1 on host")
      cmd = "nsenter -t $(docker inspect --format '{{ .State.Pid }}' "+self.id\
             +") -n tc class add dev "+self.iface+" parent "+self.root_handle\
             +" classid "+self.root_handle+self.class_handle+" htb rate "\
             +str(self.out_bw)+"kbps"
      self.system_cmd(cmd, "unable to execute tc command2 on host")
