#include <iostream>

#include "cpm.h"

int main(int, char**){
    cpm manager;
    const std::vector<cpm::Proc> processes = manager.get_all_proc();
    manager.print_table(processes);
    return 0;
}
