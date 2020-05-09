## async ping 
1. use `popen` and `WEXITSTATUS(pclose(pipe))` to get exit code
2. use future array  and  `valid()` to check avaialbe future
```
   vecFutures.push_back(std::async(std::launch::async, &exec, cmds[i].c_str()));
    if(vecFutures[i].valid()) {
```

3. Future doesn't have to be moved. (automatic move?) 

## Future improvement
- allow dynamic number of ips instead of a fixed number of address defined 
- A result vector includes <cmd, result> instead of only result.(How: store a pair type before running). e.g. Struct task { command, future, answered, result }
Note: ownership of future needs to be moved [move future or share](https://stackoverflow.com/questions/27186233/stdfuture-as-a-parameter-to-a-function-c)


## final performance
- ping 254 ip address: 1.2 seconds

