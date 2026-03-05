#include <unordered_map>

#pragma once

class sysinfo
{
public:
	sysinfo();
	~sysinfo() = default;

	struct ProcessState {
        unsigned long long total_ticks = 0;
        std::chrono::steady_clock::time_point last_read_time;
    };

	double get_used_mem_gig();
	double get_total_mem();

    double get_cpu_percent(int pid);


private:
    std::unordered_map<int, ProcessState> cpu_tracker;
};
