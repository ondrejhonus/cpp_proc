#include "sysinfo.hpp"

#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "manager.hpp"

sysinfo::sysinfo() = default;

unsigned long long get_process_ticks(int pid) {
  std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
  if (!stat_file.is_open()) return 0;

  std::string line;
  std::getline(stat_file, line);

  size_t rparen = line.find_last_of(')');
  if (rparen == std::string::npos) return 0;

  std::string stats = line.substr(rparen + 2);
  std::istringstream iss(stats);
  std::string dummy;
  unsigned long long utime = 0, stime = 0;

  for (int i = 0; i < 11; ++i) iss >> dummy;
  iss >> utime >> stime;

  return utime + stime;
}

unsigned int sysinfo::get_total_mem() const {
  std::string line;
  const std::string path = "/proc/meminfo";
  std::ifstream ReadProcess(path);
  while (getline(ReadProcess, line)) {
    if (line.compare(0, 9, "MemTotal:") == 0) {
      std::stringstream ss(line);
      std::string label;
      std::string mem_total;
      ss >> label;
      ss >> mem_total;
      return std::stoi(mem_total);
    }
  }
  return 0;
}

unsigned int sysinfo::get_used_mem_gig() const {
  std::string line;
  const std::string path = "/proc/meminfo";
  std::ifstream ReadProcess(path);
  while (getline(ReadProcess, line)) {
    if (line.compare(0, 8, "MemTotal:") == 0) {
      std::stringstream ss(line);
      std::string label;
      std::string mem_free;
      ss >> label;
      ss >> mem_free;
      unsigned int mem_total = get_total_mem();
      return mem_total - std::stoi(mem_free);
    }
  }
  return 0;
}

double sysinfo::get_cpu_percent(int pid) {
  unsigned long long current_ticks = get_process_ticks(pid);
  auto current_time = std::chrono::steady_clock::now();

  if (current_ticks == 0) return 0.0;

  ProcessState& state = cpu_tracker[pid];

  if (state.total_ticks == 0) {
    state.total_ticks = current_ticks;
    state.last_read_time = current_time;
    return 0.0;
  }

  std::chrono::duration<double> elapsed = current_time - state.last_read_time;
  unsigned long long tick_diff = current_ticks - state.total_ticks;
  long hertz = sysconf(_SC_CLK_TCK);

  double cpu_usage =
      (static_cast<double>(tick_diff) / hertz) / elapsed.count() * 100.0;

  long num_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
  cpu_usage /= num_of_cores;

  state.total_ticks = current_ticks;
  state.last_read_time = current_time;

  return cpu_usage;
}
