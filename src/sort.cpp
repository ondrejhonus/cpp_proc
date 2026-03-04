#include "sort.hpp"
#include <string>

sorting::sorting() = default;

void swap(cpm::Proc* a, cpm::Proc* b){
    cpm::Proc* temp = a;
    a = b;
    b = temp;
}

enum class Command {
    pid,
    name,
    state,
    mem,
    err
};

Command resolve_cmd(const std::string& str){
    if (str == "pid") return Command::pid;
    if (str == "name") return Command::name;
    if (str == "state") return Command::state;
    if (str == "mem") return Command::mem;
    return Command::pid;
    
}


std::vector<cpm::Proc> sorting::get_sorted_procs(std::vector<cpm::Proc>& processes, std::string method, bool is_asc)
{
    int count_of_procs = processes.size();
    for (size_t i = 0; i < count_of_procs - 1; i++)
    {
       for (size_t j = 0; j < count_of_procs - i - 1; j++)
       {
            switch (resolve_cmd(method))
            {
            case Command::pid:
                if (is_asc && processes[j].pid > processes[j + 1].pid)
                    std::swap(processes[j], processes[j + 1]);
                else if (!is_asc && processes[j].pid < processes[j + 1].pid)
                    std::swap(processes[j], processes[j + 1]);
                break;
            case Command::name:
                if (is_asc && processes[j].name > processes[j + 1].name)
                    std::swap(processes[j], processes[j + 1]);
                else if (!is_asc && processes[j].name < processes[j + 1].name)
                    std::swap(processes[j], processes[j + 1]);
                break;
            case Command::state:
            // just state alphabetically
                if (is_asc && processes[j].state > processes[j + 1].state)
                        std::swap(processes[j], processes[j + 1]);
                    else if (!is_asc && processes[j].state < processes[j + 1].state)
                        std::swap(processes[j], processes[j + 1]);
                    break;
            default:
                break;
            }
       }  
    }
    return processes;
}