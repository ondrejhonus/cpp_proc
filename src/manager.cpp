#include "manager.hpp"

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "sort.hpp"
#include "sysinfo.hpp"

using std::string;
sysinfo sys_info;

std::vector<ProcessManager::Proc> ProcessManager::get_all_proc(
    std::string method, bool is_asc) {
  std::vector<ProcessManager::Proc> procs;
  sorting sorter;
  for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
    if (!entry.is_directory()) {
      continue;
    }
    string filename = entry.path().filename().string();
    if (!is_num(filename)) {
      continue;
    }
    ProcessManager::Proc proc = parse_proc_data(filename);
    procs.push_back(proc);
  }
  auto sorted_procs = sorter.get_sorted_procs(procs, method, is_asc);
  return sorted_procs;
}

bool ProcessManager::kill_proc(int pid, int sig) {
  if (sig > 0 && sig <= 15) {
    return kill(pid, sig) == 0;
  }
  return false;
}

bool ProcessManager::is_num(string& name) {
  for (char ch : name) {
    if (!isdigit(ch)) {
      return false;
    }
  }
  return true;
}

ProcessManager::Proc ProcessManager::parse_proc_data(string& pid) {
  ProcessManager::Proc proc;
  string line;
  string filename = "/proc/" + pid + "/status";
  proc.memory = 0;
  proc.name = "";
  proc.state = "";
  proc.pid = stoi(pid);

  std::ifstream ReadProcess(filename);
  while (getline(ReadProcess, line)) {
    if (line.compare(0, 6, "VmRSS:") == 0) {
      std::stringstream ss(line);
      string label;
      ss >> label;
      ss >> proc.memory;
    } else if (line.compare(0, 5, "Name:") == 0) {
      std::stringstream ss(line);
      string label;
      ss >> label;
      ss >> proc.name;
    } else if (line.compare(0, 6, "State:") == 0) {
      std::stringstream ss(line);
      string label;
      ss >> label;
      string state_short;
      ss >> state_short;
      string state;
      ss >> state;
      // remove ()
      state.erase(0, 1);
      state.erase(state.size() - 1);
      proc.state = state;
    }
  }
  proc.cpu_percent = sys_info.get_cpu_percent(proc.pid);
  return proc;
}