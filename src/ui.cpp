#include "ui.h"
#include <memory>
#include <string>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"

#include "../include/cpm.h"

using namespace ftxui;

int ui::draw_ui() {
    cpm manager;
    auto screen = ScreenInteractive::Fullscreen();
    
    int selected_row = 0; 
    int total_rows = 0;

    auto table_content = Renderer([&]() {
        auto procs = manager.get_all_proc();
        total_rows = procs.size();

        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < total_rows; ++i) {
            const auto& p = procs[i];
            rows.push_back({
                std::to_string(p.pid), 
                p.name, 
                p.state, 
                std::to_string(p.memory) + " KB"
            });
        }

        auto table = Table(rows);
        table.SelectAll().Border(LIGHT);
        table.SelectAll().Separator(LIGHT);

        table.SelectAll().DecorateCells(center);

        table.SelectRow(0).Decorate(bold);
        table.SelectRow(0).SeparatorVertical(LIGHT);

        if (total_rows > 0) {
            selected_row = std::max(0, std::min(selected_row, total_rows - 1));

            table.SelectRow(selected_row).DecorateCells(inverted);

            table.SelectRow(selected_row).Decorate(focus);
        }

        return table.Render() | vscroll_indicator | yframe | flex;
    });

    auto header_content = Renderer([&] {
         std::vector<std::vector<std::string>> header_data;
         header_data.push_back({" [PID] ", "               [NAME] ", "                [STATE] ", " [MEMORY] "});
         auto table = Table(header_data);
         table.SelectRow(0).Decorate(bold);
         return table.Render();
    });

    auto layout = Renderer(table_content, [&] {
        return vbox({
            text("CPM - Task Manager") | center | bold,
            separator(),
            header_content->Render(), 
            separator(),
            table_content->Render() | flex,
            separator(),
            text("Arrows to Scroll | Q to Quit") | center
        }) | border;
    });

    auto final_ui = CatchEvent(layout, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }

        if (event.is_mouse()) {
            if (event.mouse().button == Mouse::WheelUp) {
                if (selected_row > 0) selected_row--;
                return true;
            }
            if (event.mouse().button == Mouse::WheelDown) {
                if (selected_row < total_rows - 1) selected_row++;
                return true;
            }
        }

        if (event == Event::ArrowUp) {
            if (selected_row > 0) selected_row--;
            return true;
        }
        if (event == Event::ArrowDown) {
            if (selected_row < total_rows - 1) selected_row++;
            return true;
        }

        return false;
    });

    screen.Loop(final_ui);
    return 0;
}