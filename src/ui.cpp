#include "ui.h"
#include <memory>
#include <string>
#include <vector>
#include <signal.h>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"

#include "../include/cpm.h"

using namespace ftxui;

auto button_style = ButtonOption::Animated();
bool modal_shown = false;
cpm proc_man;

/*
    SIGHUP		1	// Hangup.
    SIGINT		2	// Interactive attention signal.
    SIGQUIT		3	// Quit.
    SIGILL		4	// Illegal instruction.
    SIGTRAP		5	// Trace/breakpoint trap.
    SIGABRT		6	// Abnormal termination.
    SIGBUS		7	// Bus error.
    SIGFPE		8	// Erroneous arithmetic operation.
    SIGKILL		9	// Killed.
    SIGUSR1		10	// User-defined signal 1.
    SIGSEGV		11	// Invalid access to storage.
    SIGUSR2		12	// User-defined signal 2.
    SIGPIPE		13	// Broken pipe.
    SIGALRM		14	// Alarm clock.
    SIGTERM		15	// Termination request.
*/

Component ModalComponent(
                        std::function<void()> sighup,
                        std::function<void()> sigint,
                        std::function<void()> sigquit,
                        std::function<void()> sigill,
                        std::function<void()> sigtrap,
                        std::function<void()> sigabrt,
                        std::function<void()> sigbus,
                        std::function<void()> sigfpe,
                        std::function<void()> sigkill,
                        std::function<void()> sigusr1,
                        std::function<void()> sigegv,
                        std::function<void()> sigusr2,
                        std::function<void()> sigpipe,
                        std::function<void()> sigalrm,
                        std::function<void()> sigterm,
                        std::function<void()> hide_modal
                        )
{
    auto component = Container::Vertical({
        Button("[0] Cancel", hide_modal, button_style),
        Button("[1] SIGHUP", sighup, button_style),
        Button("[2] SIGINT", sigint, button_style),
        Button("[3] SIGQUIT", sigquit, button_style),
        Button("[4] SIGILL", sigill, button_style),
        Button("[5] SIGTRAP", sigtrap, button_style),
        Button("[6] SIGABRT", sigabrt, button_style),
        Button("[7] SIGBUS", sigbus, button_style),
        Button("[8] SIGFPE", sigfpe, button_style),
        Button("[9] SIGKILL", sigkill, button_style),
        Button("[10] SIGUSR1", sigusr1, button_style),
        Button("[11] SIGUSEGV", sigegv, button_style),
        Button("[12] SIGUSR2", sigusr2, button_style),
        Button("[13] SIGPIPE", sigpipe, button_style),
        Button("[14] SIGALRM", sigalrm, button_style),
        Button("[15] SIGTERM", sigterm, button_style),

    });
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

int pidof_selected(int selected_row)
{
    auto procs = proc_man.get_all_proc();
    return procs[selected_row].pid;
}

void handle_proc_kill(int selected_row) {
    
}

int ui::draw_ui()
{
    auto screen = ScreenInteractive::Fullscreen();

    int selected_row = 0;
    int total_rows = 0;

    auto table_content = Renderer([&]()
                                  {
        auto procs = proc_man.get_all_proc();
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
    auto sighup = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGHUP);};
    auto sigint = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGINT);};
    auto sigquit = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGQUIT);};
    auto sigill = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGILL);};
    auto sigtrap = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGTRAP);};
    auto sigabrt = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGABRT);};
    auto sigbus = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGBUS);};
    auto sigfpe = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGFPE);};
    auto sigkill = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGKILL);};
    auto sigusr1 = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGUSR1);};
    auto sigev = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGSEGV);};
    auto sigusr2 = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGUSR2);};
    auto sigpipe = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGPIPE);};
    auto sigalrm = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGALRM);};
    auto sigterm = [&] {proc_man.kill_proc(pidof_selected(selected_row), SIGTERM);};

    auto exit = screen.ExitLoopClosure();
    auto do_nothing = [&] {};

    Component final_ui = layout;
    auto modal_component = ModalComponent(sighup, sigint, sigquit, sigill, sigtrap, sigabrt, sigbus, sigfpe, sigkill, sigusr1, sigev, sigusr2, sigpipe, sigalrm, sigterm, hide_modal);

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