#!/usr/bin/env python

import os
import sys
import tempfile
import time
import subprocess

USAGE_CHECK_PERIOD = 1
# RX_BYTES = "/sys/class/net/eth0/statistics/rx_bytes"
# TX_BYTES = "/sys/class/net/eth0/statistics/tx_bytes"
RX_BYTES = "/proc/<pid>/net/dev"

def cpu(pid):
    return sum(map(float, file('/proc/%s/stat' % pid).read().split()[13:17]))

# def net(path):
#     return float(file(path).read())

def system_cmd(cmd):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    # if only the command executed successfully
    if p.wait() == 0:
      return p.stdout.readlines()
    else:
      raise Exception("error!")

def net(pid, col):
    return int(system_cmd("cat /proc/"+str(pid)+"/net/dev | awk '/eth0/ {print $"+str(col)+"}'")[0])

def main():
    if len(sys.argv) < 2 or len(sys.argv) > 2 or sys.argv[1] == '-h':
        print 'log.py pid'
        return

    pid = sys.argv[1]
    cpu_stats = []
    rx_stats = []
    tx_stats = []
    cpu_prev = cpu(pid)
    rx_prev = new = net(pid, 2)
    tx_prev = new = net(pid, 10)
    # rx_prev = net(RX_BYTES)
    # tx_prev = net(TX_BYTES)

    try:
        while True:
            time.sleep(USAGE_CHECK_PERIOD)
            # CPU
            new = cpu(pid)
            print ((new-cpu_prev)/USAGE_CHECK_PERIOD)
            cpu_prev = new
            # RX
            # new = net(RX_BYTES)
            new = net(pid, 2)
            print ((new-rx_prev)/USAGE_CHECK_PERIOD)
            rx_prev = new
            # TX
            # new = net(TX_BYTES)
            new = net(pid, 10)
            print ((new-tx_prev)/USAGE_CHECK_PERIOD)
            tx_prev = new
    except:
        pass

    t1, t2, t3 = "cpu_stats.txt", "rx_stats.txt", "tx_stats.txt"
    f1, f2, f3 = file(t1, 'w'), file(t2, 'w'), file(t3, 'w')

    print
    print 'cpu data: %s' % t1
    print >>f1, '\n'.join(map(str, cpu_stats))
    print 'rx data: %s' % t2
    print >>f2, '\n'.join(map(str, rx_stats))
    print 'tx data: %s' % t3
    print >>f3, '\n'.join(map(str, tx_stats))

if __name__ == '__main__':
    main()
