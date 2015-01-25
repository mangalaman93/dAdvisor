# Guest abstract class
# provides functions to set/get
#  network and CPU allocation/usage

import os
import subprocess
import sys

class Guest(object):
  """Abstract class for Virtualized Platforms"""

  def __init__(self, name):
    super(Guest, self).__init__()
    self.name = name
    self.in_bw = None
    self.out_bw = None

  # compare two guests "=="
  def __eq__(self, other):
    if isinstance(other, self.__class__):
      return self.name == other.name
    return False

  # compare two containers "!="
  def __ne__(self, other):
    if isinstance(other, self.__class__):
      return self.name != other.name
    return True

  # execute system command and return output
  def system_cmd(self, cmd, message):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    # if only the command executed successfully
    if p.wait() == 0:
      return p.stdout.readlines()[0]
    else:
      raise Exception(message)

  # check whether a directory exists
  def exists(self, gdir):
    if not os.path.isdir(gdir):
      raise Exception("guest doesn't exist!")

  # get cpu usage
  def get_cpu_usage(self):
    """cpu usage by the guest"""
    raise NotImplementedError

  # get current allocated CPU shares
  def get_soft_cpu_shares(self):
    """current allocated CPU shares (soft limit)"""
    raise NotImplementedError

  # get hard limit on cpu shares
  def get_hard_cpu_shares(self):
    """current allocated CPU shares (hard limit)"""
    raise NotImplementedError

  # get maximum number of CPUs allowed to be used (CPU pinning)
  def get_pinned_cpus(self):
    """maximum number of CPUs that can
       possibly be used by the guest"""
    raise NotImplementedError

  # received network traffic rate
  def get_network_in_usage(self):
    """received network traffic usage (rate)"""
    raise NotImplementedError

  # sent network traffic rate
  def get_network_out_usage(self):
    """sent network traffic usage (rate)"""
    raise NotImplementedError

  # set given value of cpu shares for the container
  def set_soft_cpu_shares(self, shares):
    """set current allocated CPU shares (soft limit)"""
    raise NotImplementedError

  # set allocated CPU to the container (shares=1024=>1core)
  def set_hard_cpu_shares(self, shares):
    """set current allocated CPU shares (hard limit)"""
    raise NotImplementedError

  # set network-in bandwidth, bw in bytes/sec
  def set_network_in_bw(self, bw):
    """set limit on received traffic on network (eth0)"""
    raise NotImplementedError

  # set network-out bandwidth, bw in bytes/sec
  def set_network_out_bw(self, bw):
    """set limit on sent traffic on network (eth0)"""
    raise NotImplementedError
