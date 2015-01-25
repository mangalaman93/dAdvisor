# KVM class
# allows to get usage statistics for a KVM VMs
# and change resource allocation for the VMs

import subprocess
import sys

class KvmVM(object):
  """kvm VM class"""

  def __init__(self, name):
    super(KvmVM, self).__init__()
    self.name = name
