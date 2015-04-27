#!/bin/sh

# checking command line arguments
if [ "$#" -ne 1 ]; then
  echo "error!"
  echo "Usage: $0 <ip-address>" >&2
  exit 1
fi

# command line arguments
IP_ADDRESS=$1
PORT=8888

# following scenario is emulated here
#           <---20s--> <---20s--> <---20s-->
#    3000              __________
#                     |          |
#    1500   __________|          |__________
#          |                                |
#     0  __|                                |__
#
bash -c "echo \"cset rate 1500\" >/dev/udp/$IP_ADDRESS/$PORT"
echo "set rate to 1500..."
sleep 30s
bash -c "echo \"cset rate 3000\" >/dev/udp/$IP_ADDRESS/$PORT"
echo "set rate to 3000..."
sleep 30s
bash -c "echo \"cset rate 1500\" >/dev/udp/$IP_ADDRESS/$PORT"
echo "set rate to 1500..."
sleep 30s
bash -c "echo \"q\" > /dev/udp/$IP_ADDRESS/$PORT"
