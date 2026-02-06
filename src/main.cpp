#include <iostream>

#include "../include/cpm.h"
#include "../include/ui.h"

int main(int, char**){
    // cpm manager;
    // const std::vector<cpm::Proc> processes = manager.get_all_proc();
    // manager.print_table(processes);

    ui UI;
    UI.draw_ui();
    return 0;
}
