#!/bin/sh

# checking command line arguments
if [ "$#" -lt 1 ]; then
  echo "error!"
  echo "Usage: $0 <ip-address1> <ip-address2> ..." >&2
  exit 1
fi

# command line arguments
PORT=8888

# following scenario is emulated here
#           <---20s--> <---20s--> <---20s-->
#    3000              __________
#                     |          |
#    1500   __________|          |__________
#          |                                |
#     0  __|                                |__
#
for ip in "$@"
do
  bash -c "echo \"cset rate 1500\" >/dev/udp/$ip/$PORT"
  echo "set rate to 1500..."
  sleep 30s
  bash -c "echo \"cset rate 3000\" >/dev/udp/$ip/$PORT"
  echo "set rate to 3000..."
  sleep 30s
  bash -c "echo \"cset rate 1500\" >/dev/udp/$ip/$PORT"
  echo "set rate to 1500..."
  sleep 30s
  bash -c "echo \"q\" > /dev/udp/$ip/$PORT"
done
