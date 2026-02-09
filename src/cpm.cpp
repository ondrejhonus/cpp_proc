#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sstream>
#include <iomanip>

#include "../include/cpm.h"
#include "cpm.h"
#include <signal.h>
#include <sys/types.h>

using std::string;

std::vector<cpm::Proc> cpm::get_all_proc()
{
    std::vector<cpm::Proc> procs;
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
    return procs;
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
    // #define	SIGHUP		1	/* Hangup.  */
    // #define	SIGINT		2	/* Interactive attention signal.  */
    // #define	SIGQUIT		3	/* Quit.  */
    // #define	SIGILL		4	/* Illegal instruction.  */
    // #define	SIGTRAP		5	/* Trace/breakpoint trap.  */
    // #define	SIGABRT		6	/* Abnormal termination.  */
    // #define SIGBUS		 7	/* Bus error.  */
    // #define	SIGFPE		8	/* Erroneous arithmetic operation.  */
    // #define	SIGKILL		9	/* Killed.  */
    // #define SIGUSR1		10	/* User-defined signal 1.  */
    // #define	SIGSEGV		11	/* Invalid access to storage.  */
    // #define SIGUSR2		12	/* User-defined signal 2.  */
    // #define	SIGPIPE		13	/* Broken pipe.  */
    // #define	SIGALRM		14	/* Alarm clock.  */
    // #define	SIGTERM		15	/* Termination request.  */

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
            state.erase(0, 1);
            state.erase(state.size() - 1);
            proc.state = state;
        }
    }
    return proc;
}
