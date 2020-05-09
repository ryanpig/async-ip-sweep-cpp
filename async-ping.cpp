/*
 * This program takes advantage of asynchronous calls in C++, achiving concurrency.
 * [use popen with stdout or pclose with returned status.](https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po)
 * [return exit status by using WEXITSTATUS](https://stackoverflow.com/questions/15058876/how-to-capture-the-exit-code-and-stderr-of-the-command-that-is-run-in-c)
 */

#include <iostream>
#include <future>
#include <vector>
#include <algorithm>

static std::string RESP_REACHABLE = "reachable";
static std::string RESP_UNREACHABLE = "unreachable";   
static constexpr int NUMBER_IP = 254;

auto exec(const char* cmd) {
  // std::array<char, 128> buffer;
  std::string result;

  // return standard I/O stream int
  auto pipe = popen(cmd, "r");
  if(!pipe) throw std::runtime_error("popen() failed");

  // while(!feof(pipe)) {
  //   if(fgets(buffer.data(), 128, pipe) != nullptr)
  //       result += buffer.data();
  // }
  // 0: reachable
  return WEXITSTATUS(pclose(pipe)) ?  RESP_UNREACHABLE : RESP_REACHABLE;
}
  
auto running_loop(std::vector<std::string> cmds) {
  std::vector<std::future<std::string>> vecFutures;
  for(std::size_t i = 0; i < cmds.size(); i++) {
    vecFutures.push_back(std::async(std::launch::async, &exec, cmds[i].c_str()));
  }
  
  std::array<std::string, NUMBER_IP> results; 
  std::array<bool, NUMBER_IP> answered{false};
  auto AllAnswered = [&answered](){
    return std::all_of(answered.begin(), answered.end(), 
        [](bool b){return b;}
        );
  };
  // use while loop to check each future's status
  while(!AllAnswered()) {
    for(size_t i = 0; i < vecFutures.size(); i++) {
      if(!answered[i] && vecFutures[i].wait_for(std::chrono::milliseconds(10)) ==  std::future_status::ready) {
          if(vecFutures[i].valid()) {
            results[i] = vecFutures[i].get();
            answered[i] = true;
          }
        }
    }
  }

  return results;
}

int main()
{
  std::string pingCommand = "ping -c 1 -W 1 ";
  std::string ip = "localhost";
  std::string subnet = "172.17.100.";

  std::vector<std::string> cmds;
  for(auto i = 1; i <= NUMBER_IP; i++) {
    cmds.push_back(pingCommand + subnet + std::to_string(i));
  }
  
  auto results = running_loop(cmds);
  for(size_t i = 0; i < NUMBER_IP; i++) { 
    if(results[i] == RESP_REACHABLE)
      std::cout << cmds[i] << ", status:" << results[i] << std::endl; 
  }

  // old way (blocking calls)
  // int cntReachable{0};
  // int cntUnreachable{0}; 
  // for(auto i = 1; i < 10; i++) {
  //   std::string cmd = pingCommand + subnet + std::to_string(i);
  //   auto  r = exec((cmd).c_str());
  //   if(r == RESP_REACHABLE) cntReachable++;  
  //   if(r == RESP_UNREACHABLE) cntUnreachable++;  
  //   std::cout << subnet + std::to_string(i) << ", status: "<< r << std::endl;
  // }
  // std::cout << "Total:" << cntReachable + cntUnreachable << ", Yes:" << cntReachable << ", No:" << cntUnreachable << '\n';

  return 0;
}

