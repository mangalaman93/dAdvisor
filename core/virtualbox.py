# virtualbox class
# allows to get usage statistics for a virtualbox VMs
# and change resource allocation for the VMs

from guest import *

# @todo
class VirtualBoxVM(Guest):
  """VirtualBox VM class"""

  def __init__(self, name):
    super(VirtualBoxVM, self).__init__()
    self.name = name
