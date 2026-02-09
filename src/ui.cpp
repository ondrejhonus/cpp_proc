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

auto button_style = ButtonOption::Animated();
bool modal_shown = false;

// Definition of the main component. The details are not important.
Component MainComponent(std::function<void()> show_modal,
                        std::function<void()> exit)
{
    auto component = Container::Vertical({
        Button("Show modal", show_modal, button_style),
        Button("Quit", exit, button_style),
    });
    // Polish how the two buttons are rendered:
    component |= Renderer([&](Element inner)
                          {
                              return vbox({
                                         text("Main component"),
                                         separator(),
                                         inner,
                                     })                               //
                                     | size(WIDTH, GREATER_THAN, 15)  //
                                     | size(HEIGHT, GREATER_THAN, 15) //
                                     | border                         //
                                     | center;                        //
                          });
    return component;
}

// Definition of the modal component. The details are not important.
Component ModalComponent(std::function<void()> do_nothing,
                         std::function<void()> hide_modal)
{
    auto component = Container::Vertical({
        Button("Do nothing", do_nothing, button_style),
        Button("Quit modal", hide_modal, button_style),
    });
    // Polish how the two buttons are rendered:
    component |= Renderer([&](Element inner)
                          {
                              return vbox({
                                         text("Modal component "),
                                         separator(),
                                         inner,
                                     })                              //
                                     | size(WIDTH, GREATER_THAN, 30) //
                                     | border;                       //
                          });
    return component;
}

int ui::draw_ui()
{
    cpm manager;
    auto screen = ScreenInteractive::Fullscreen();

    int selected_row = 0;
    int total_rows = 0;

    auto table_content = Renderer([&]()
                                  {
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

        return table.Render() | vscroll_indicator | yframe | flex; });

    auto header_content = Renderer([&]
                                   {
         std::vector<std::vector<std::string>> header_data;
         header_data.push_back({
            " [PID] ", 
            "               [NAME] ", 
            "                [STATE] ", 
            " [MEMORY] "
        });
         auto table = Table(header_data);
         table.SelectRow(0).Decorate(bold);
         return table.Render(); });

    auto layout = Renderer(table_content, [&]
                           { return vbox({text("CPM - Task Manager") | center | bold,
                                          separator(),
                                          header_content->Render(),
                                          separator(),
                                          table_content->Render() | flex,
                                          separator(),
                                          text("[q]uit | [k]ill process") | center}) |
                                    border; });

    auto show_modal = [&]
    { modal_shown = true; };
    auto hide_modal = [&]
    { modal_shown = false; };
    auto exit = screen.ExitLoopClosure();
    auto do_nothing = [&] {};

    Component final_ui = layout;
    auto modal_component = ModalComponent(do_nothing, hide_modal);

    final_ui |= Modal(modal_component, &modal_shown);

    // Catch events on the composed UI:
    final_ui = CatchEvent(final_ui, [&](Event event)
                          {
        if (event == Event::Character('q'))
        {
            screen.Exit();
            return true;
        }
        if (event == Event::ArrowUp || (event.is_mouse() && event.mouse().button == Mouse::WheelUp))
        {
            if (selected_row > 0 && !modal_shown)
            {
                selected_row--;
                return true;
            }
            else
            {
                return false;
            }
        }
        if (event == Event::ArrowDown || (event.is_mouse() && event.mouse().button == Mouse::WheelDown)) {
        if (selected_row < total_rows - 1 && !modal_shown)
        {
            selected_row++;
            return true;
        }
        // else -> default
        else
        {
            return false;
        }
        }

        if (event == Event::Character('k')) {
        modal_shown == true ? hide_modal() : show_modal();
        return true;
        }

        return false; });

    screen.Loop(final_ui);
    return 0;
}