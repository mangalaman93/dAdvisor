# virtualbox class
# allows to get usage statistics for a virtualbox VMs
# and change resource allocation for the VMs

from guest import *

# parameters
USAGE_CHECK_PERIOD = 1
NUM_CUR_SAMPLES = 5

class VirtualBoxVM(Guest):
  """VirtualBox VM class"""

  def __init__(self, name):
    super(VirtualBoxVM, self).__init__(name)
    self.name = name
    cmd = "VBoxManage metrics setup --period {} --samples {} {} \
          CPU/Load".format(USAGE_CHECK_PERIOD, NUM_CUR_SAMPLES, self.name)
    self.system_cmd(cmd, "unable to setup metrics collection!")

  # get current cpu usage
  def get_cpu_usage(self):
    pass

  # get current allocated CPU shares
  def get_soft_cpu_shares(self):
    pass

  # get hard limit on cpu shares
  def get_hard_cpu_shares(self):
    pass

  # get maximum number of CPUs allowed to be used (CPU pinning)
  def get_pinned_cpus(self):
    pass

  # received network traffic rate
  def get_network_in_usage(self):
    pass

  # sent network traffic rate
  def get_network_out_usage(self):
    pass

  # change given value of cpu shares for the container
  def set_soft_cpu_shares(self, shares):
    pass

  # change allocated CPU to the container (shares=1024=>1core)
  def set_hard_cpu_shares(self, shares):
    pass

  # change network-in bandwidth, bw in bytes/sec
  def set_network_in_bw(self, bw):
    pass

  # change network-out bandwidth, bw in bytes/sec
  def set_network_out_bw(self, bw):
    pass
