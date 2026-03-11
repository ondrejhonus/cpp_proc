#include "ui.hpp"

#include <signal.h>

#include <memory>
#include <string>
#include <vector>

#include "keybinds.hpp"
#include "manager.hpp"
#include "sort.hpp"

using namespace ftxui;

#define TOTAL_COLS 5

bool modal_shown = false;

ui::TableInfo table_info;
ProcessManager process_manager;
auto processes = process_manager.get_all_proc(table_info.sorting_method,
                                              table_info.sorting_is_asc);

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

Component ModalComponent(ui::ModalOptions options) {
  auto component = Container::Vertical({
      Button("[0] Cancel", options.hide_modal, ui::button_style),
      Button("[1] SIGHUP", options.sighup, ui::button_style),
      Button("[2] SIGINT", options.sigint, ui::button_style),
      Button("[3] SIGQUIT", options.sigquit, ui::button_style),
      Button("[4] SIGILL", options.sigill, ui::button_style),
      Button("[5] SIGTRAP", options.sigtrap, ui::button_style),
      Button("[6] SIGABRT", options.sigabrt, ui::button_style),
      Button("[7] SIGBUS", options.sigbus, ui::button_style),
      Button("[8] SIGFPE", options.sigfpe, ui::button_style),
      Button("[9] SIGKILL", options.sigkill, ui::button_style),
      Button("[10] SIGUSR1", options.sigusr1, ui::button_style),
      Button("[11] SIGUSEGV", options.sigegv, ui::button_style),
      Button("[12] SIGUSR2", options.sigusr2, ui::button_style),
      Button("[13] SIGPIPE", options.sigpipe, ui::button_style),
      Button("[14] SIGALRM", options.sigalrm, ui::button_style),
      Button("[15] SIGTERM", options.sigterm, ui::button_style),

  });
  component |= Renderer([&](Element inner) {
    return vbox({
               text("Select signal to send:") | center | bold,
               separator(),
               inner,
           }) |
           size(WIDTH, GREATER_THAN, 30) | frame | vscroll_indicator |
           size(HEIGHT, LESS_THAN, 30) | border;
  });
  return component;
}

int pidof_selected(int selected_row) {
  processes = process_manager.get_all_proc(table_info.sorting_method,
                                           table_info.sorting_is_asc);
  return processes[selected_row].pid;
}

int ui::draw_ui() {
  auto screen = ScreenInteractive::TerminalOutput();

  auto table_content = create_table(processes, table_info);
  auto layout = Renderer(table_content, [&] {
    return vbox({text("CPM - Task Manager") | center | bold, separator(),
                 table_content->Render(), separator(),
                 text("[q]uit | [Shift]+[k]ill process") | center}) |
           border;
  });

  // define kill proc. menu
  auto show_modal = [&] { modal_shown = true; };
  auto hide_modal = [&] { modal_shown = false; };

  auto sighup = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGHUP);
  };
  auto sigint = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGINT);
  };
  auto sigquit = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGQUIT);
  };
  auto sigill = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGILL);
  };
  auto sigtrap = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGTRAP);
  };
  auto sigabrt = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGABRT);
  };
  auto sigbus = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGBUS);
  };
  auto sigfpe = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGFPE);
  };
  auto sigkill = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGKILL);
  };
  auto sigusr1 = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGUSR1);
  };
  auto sigev = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGSEGV);
  };
  auto sigusr2 = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGUSR2);
  };
  auto sigpipe = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGPIPE);
  };
  auto sigalrm = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGALRM);
  };
  auto sigterm = [&] {
    process_manager.kill_proc(pidof_selected(table_info.selected_row), SIGTERM);
  };

  auto exit = screen.ExitLoopClosure();

  Component final_ui = layout;
  auto modal_component = ModalComponent(ui::ModalOptions{});

  final_ui |= Modal(modal_component, &modal_shown);

  // keybinds
  final_ui = CatchEvent(final_ui, [&](Event event) {
    return keybinds::handle_events(event, table_info, TOTAL_COLS, modal_shown,
                                   show_modal, hide_modal, screen);
  });

  screen.Loop(final_ui);
  return 0;
}

void ui::async_post_event(Event event) {
  std::thread([event] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ScreenInteractive::Active()->PostEvent(event);
  }).detach();
}

ftxui::Component ui::create_table(std::vector<ProcessManager::Proc>& processes,
                                  ui::TableInfo& table_info) {
  return Renderer([&]() {
    processes = process_manager.get_all_proc(table_info.sorting_method,
                                             table_info.sorting_is_asc);
    table_info.total_rows = processes.size();
    std::vector<std::vector<std::string>> rows;
    for (int i = 0; i < table_info.total_rows; ++i) {
      if (i == 0) {
        rows.push_back({"[PID]", "[NAME]", "[STATE]", "[MEMORY]", "[CPU%]"});
      }
      const auto& p = processes[i];
      rows.push_back({std::to_string(p.pid), p.name, p.state,
                      std::to_string(p.memory) + " KB",
                      std::to_string(p.cpu_percent) + " %"});
    }

    auto table = Table(rows);
    table.SelectAll().Border(LIGHT);
    table.SelectAll().Separator(LIGHT);
    table.SelectAll().DecorateCells(center);
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);

    // flex size on all columnds based on terminal size
    table.SelectColumns(0, TOTAL_COLS - 1).Decorate(flex_grow);

    if (table_info.total_rows > 0) {
      table_info.selected_row = std::max(
          0, std::min(table_info.selected_row, table_info.total_rows + 1));
      if (table_info.selected_row != 0) {
        table.SelectRow(table_info.selected_row).DecorateCells(inverted);
        table.SelectRow(table_info.selected_row).Decorate(focus);
      }
      table.SelectCell(table_info.selected_col, 0).Decorate(inverted);
      table.SelectCell(table_info.selected_col, 0).Decorate(focus);

      switch (table_info.selected_col) {
        case 0:
          table_info.sorting_method = "pid";
          break;
        case 1:
          table_info.sorting_method = "name";
          break;
        case 2:
          table_info.sorting_method = "state";
          break;
        case 3:
          table_info.sorting_method = "mem";
          break;
        case 4:
          table_info.sorting_method = "cpu";
          break;
      }
    }

    return table.Render() | vscroll_indicator | yframe | flex;
  });
}