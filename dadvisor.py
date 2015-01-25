#!/usr/bin/python

from core.container import *
from core.monitor import *

# commands
CMD_USAGE = ''' usage: <command> [<args>]
commands:
  add <id/name>  monitor docker container
  rm  <id/name>  stop monitoring the container'''
monitorThread = Monitor()

# main logic for reading input from command line
# example commands
#  add <id/name>
#  rm  <id/name>
def read_input():
  while True:
    try:
      cmd = raw_input("dadvisor:~$ ")
      cmd_list = cmd.split()

      # exit if no input given
      if cmd_list == []:
        break

      # add command
      if cmd_list[0] == "add" and len(cmd_list)==2:
        monitorThread.add_container(Container(cmd_list[1]))
        print "added container"
      # rm command
      elif cmd_list[0] == "rm" and len(cmd_list)==2:
        monitorThread.rm_container(Container(cmd_list[1]))
        print "removed container"
      # otherwise
      else:
        print "**Error: unrecognized command!"
        print CMD_USAGE

    except Exception, e:
      print "**Error:",e

# main program, creates both threads
monitorThread.start()
read_input()
monitorThread.stop()
