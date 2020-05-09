## Description
Inspired by Python `asyncio`, `async-ip-sweep-cpp` can also take advantage of asynchronous calls by C++ `async` w/ command-line `ping` command, which achieves high concurrency to scan all avaiable ip addresses in the subnet. The performance is similar to `nmap` that scan the same ip range by sending ICMP requests. Adding hostname resolution of reachable IPs, however, makes this tool much useful as it only uses the third of execution time than nmap.

## Usage
`make && ./async-ip-sweep-cpp` (w/ hostname resolution) 
`make && ./async-ip-sweep-cpp -n` (w/o hostname resolution)
Note: change subnet in the cpp file

## Performance
##### async-ip-sweep-cpp
Condition: 35 avaiable IP addresses of 254 IP addresses 
- w/o hostname resolution: 1.17 seconds
- w/ hostname resolution: 1.26 seconds

##### nmap 
- w/o hostname resolution: 1.21 seconds (`sudo nmap -sn -PE <ip range>`)
- w/ hostname resolution: 3.34 seconds (`sudo nmap -n -sn -PE <ip range>`)

