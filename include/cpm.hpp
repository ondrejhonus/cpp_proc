#include <string>
#include <vector>
#include <chrono>

#pragma once


class cpm
{
public:
    cpm() = default;
    ~cpm() = default;

    struct Proc
    {
        int pid;
        std::string name;
        std::string state;
        long memory;
        double cpu_percent;
    };

    std::vector<Proc> get_all_proc(std::string method, bool is_asc);

    bool kill_proc(int pid, int sig);

private:
    bool is_num(std::string& name);
    Proc parse_proc_data(std::string& pid);
};

