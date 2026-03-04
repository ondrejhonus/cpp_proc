#include <iostream>

#include "cpm.hpp"
#include "ui.hpp"

int main(int, char**){
    // cpm manager;
    // const std::vector<cpm::Proc> processes = manager.get_all_proc();
    // manager.print_table(processes);

    ui UI;
    UI.draw_ui();
    return 0;
}
