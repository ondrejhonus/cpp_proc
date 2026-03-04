#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <unistd.h>

#include "cpm.hpp"
#include "sort.hpp"
#include <signal.h>
#include <sys/types.h>
#include <unordered_map>

using std::string;

std::vector<cpm::Proc>cpm::get_all_proc(std::string method, bool is_asc)
{
    std::vector<cpm::Proc> procs;
    sorting sorter;
    for (const auto &entry : std::filesystem::directory_iterator("/proc"))
    {
        if (!entry.is_directory())
        {
            continue;
        }
        string filename = entry.path().filename().string();
        if (!is_num(filename))
        {
            continue;
        }
        cpm::Proc proc = parse_proc_data(filename);
        procs.push_back(proc);
    }
    auto sorted_procs = sorter.get_sorted_procs(procs, method, is_asc);
    return sorted_procs;
}

void cpm::print_table(const std::vector<Proc> &ps)
{
    const int nameWidth = 40;
    const int pidWidth = 10;
    const int statWidth = 8;
    const int memWidth = 12;

    std::cout << std::left << std::setw(nameWidth) << "NAME"
              << std::left << std::setw(pidWidth) << "PID"
              << std::left << std::setw(statWidth) << "STAT"
              << std::right << std::setw(memWidth) << "MEM (kB)"
              << std::endl;

    
    std::cout << std::string(nameWidth + pidWidth + statWidth + memWidth, '-') << std::endl;

    for (const auto& proc : ps)
    {
        std::cout << std::left << std::setw(nameWidth) << proc.name
                  << std::left << std::setw(pidWidth)  << proc.pid
                  << std::left << std::setw(statWidth) << proc.state
                  << std::right << std::setw(memWidth) << proc.memory
                  << std::endl;
    }
}

bool cpm::kill_proc(int pid, int sig)
{
    if (sig > 0 && sig <= 15) {
        return kill(pid, sig) == 0;
    }
    return false;
}

bool cpm::is_num(string &name)
{
    for (char ch : name)
    {
        if (!isdigit(ch))
        {
            return false;
        }
    }
    return true;
}

cpm::Proc cpm::parse_proc_data(string &pid)
{
    cpm::Proc proc;
    string line;
    string filename = "/proc/" + pid + "/status";
    proc.memory = 0;
    proc.name = "";
    proc.state = "";
    proc.pid = stoi(pid);

    std::ifstream ReadProcess(filename);
    while (getline(ReadProcess, line))
    {
        if (line.compare(0, 6, "VmRSS:") == 0)
        {
            std::stringstream ss(line);
            string label;
            ss >> label;
            ss >> proc.memory;
        }
        else if (line.compare(0, 5, "Name:") == 0)
        {
            std::stringstream ss(line);
            string label;
            ss >> label;
            ss >> proc.name;
        }
        else if (line.compare(0, 6, "State:") == 0)
        {
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
        // cpu usage
        proc.cpu_percent = calculate_cpu_usage(proc.pid);
    }
    return proc;
}

static std::unordered_map<int, cpm::ProcessState> cpu_tracker;

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

double cpm::calculate_cpu_usage(int pid) {
    unsigned long long current_ticks = get_process_ticks(pid);
    auto current_time = std::chrono::steady_clock::now();

    if (current_ticks == 0) return 0.0;

    cpm::ProcessState& state = cpu_tracker[pid];

    if (state.total_ticks == 0) {
        state.total_ticks = current_ticks;
        state.last_read_time = current_time;
        return 0.0; 
    }

    std::chrono::duration<double> elapsed = current_time - state.last_read_time;
    unsigned long long tick_diff = current_ticks - state.total_ticks;
    long hertz = sysconf(_SC_CLK_TCK);

    double cpu_usage = (static_cast<double>(tick_diff) / hertz) / elapsed.count() * 100.0;

    state.total_ticks = current_ticks;
    state.last_read_time = current_time;

    return cpu_usage;
}