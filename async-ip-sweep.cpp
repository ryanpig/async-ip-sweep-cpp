#include <iostream>
#include <future>
#include <vector>
#include <algorithm>
#include <cstring>

#include <netdb.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

static const std::string pingCommand = "ping -c 1 -W 1 ";
static const std::string RESP_REACHABLE = "reachable";
static const std::string RESP_UNREACHABLE = "unreachable";   
static const std::string subnet = "192.168.100.";
static constexpr int NUMBER_IP = 254;
static bool useResolution = true;

struct Task {
  std::string cmd;
  std::string ip;
  bool answered;
  std::future<std::string> fut;
  std::string result;
  std::string hostname;
};

std::string getHostName(const char* ipstr) {
  struct in_addr ip;
  struct hostent *hp;
  if (!inet_aton(ipstr, &ip)) {
      return "Failed IP parsing";
  }

  if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
      return "No hostname";
  }
   return std::string(hp->h_name);
}

auto taskExecution(const char* cmd, const char* ip) {
  std::array<char, 128> buffer;
  std::string result;

  auto pipe = popen(cmd, "r");
  if(!pipe) throw std::runtime_error("popen() failed");

  // read all output data of the pipe 
  while(!feof(pipe)) {
    if(fgets(buffer.data(), 128, pipe) != nullptr)
        result += buffer.data();
  }

  // DEBUG:
  // std::cout << result << std::endl;
  return WEXITSTATUS(pclose(pipe)) ?  RESP_UNREACHABLE : useResolution ? RESP_REACHABLE + " :" + getHostName(ip) : RESP_REACHABLE;
}
  
void runTasks(std::vector<Task> & tasks) {
  // run each task
  for(auto &task : tasks) {
    task.fut = std::move(std::async(std::launch::async, &taskExecution, task.cmd.c_str(), task.ip.c_str()));
  }

  // check all futures are available
  auto allAnswered = [&tasks](){
    return std::all_of(tasks.begin(), tasks.end(), 
          [](Task &t){return t.answered;}
        );
  };

  // check each future's status
  while(!allAnswered()) {
    for(auto &task : tasks) {
      if(!task.answered && task.fut.wait_for(std::chrono::milliseconds(10)) ==  std::future_status::ready) {
          if(task.fut.valid()) {
            task.result = task.fut.get();
            task.answered = true;
          }
        }
    }
  }
}

int main(int argc, char* argv[])
{
  // disable hostname resolution
  if(argc == 2 && strcmp(argv[1],"-n") == 0) {
      useResolution = false;
  }

  // create tasks running asynchronously
  std::vector<Task> tasks;
  for(auto i = 1; i <= NUMBER_IP; i++) {
    std::string ip = subnet + std::to_string(i);
    tasks.push_back( { pingCommand + ip, ip, false, {}, "", "" });
  }

  // submit all tasks 
  runTasks(tasks);
 
  // show results of reachable ip addresses
  for(const auto &task : tasks) {
    if(task.result != RESP_UNREACHABLE)
      std::cout << task.ip << ", status:" << task.result << std::endl; 
  }

  return 0;
}

