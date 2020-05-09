# This program uses async calls to ping each ip address and return reachable ip address
# Based on these ip address, looking up their hostname in the dns server
#!/bin/bash

for ip in $(./a.out | cut -d ' ' -f6 | cut -d ',' -f1);
do
  echo $ip "$(nslookup $ip | grep 'name = ' | awk -F ' = ' '{print $2}')";
done


# for ip in `seq 1 1 20`; 
# do
#   ip="172.17.100."${ip}
#   # echo $ip
#   ping -c 1 -w 1 $ip >/dev/null && echo $ip "$(nslookup $ip | grep 'name = ' | awk -F ' = ' '{print $2}')";
# done
