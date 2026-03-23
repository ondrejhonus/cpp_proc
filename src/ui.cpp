#include "ui.hpp"

#include <signal.h>

#include <chrono>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

#include "ftxui/component/animation.hpp"
#include "keybinds.hpp"
#include "manager.hpp"
#include "sort.hpp"

using namespace ftxui;

#define UPDATE_INTERVAL_MS 3000
#define TOTAL_COLS 5
#define BOTTOM_TEXT                                                            \
  "[q]uit | [s]end signal | [T]erminate | [Return/Space] ASC/DESC | [PgUp/g] " \
  "To Top | [PgUp/G] To Bottom"

bool modal_shown = false;
std::atomic<bool> app_is_running{true};

ui::TableInfo table_info;
ProcessManager process_manager;
std::vector<ProcessManager::Proc> processes = {};

int ui::draw_ui() {
  process_manager.get_all_proc("pid", true);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  auto screen = ScreenInteractive::TerminalOutput();

  auto table_content = create_table(processes, table_info);
  auto layout = Renderer(table_content, [&] {
    return vbox({text("CPM - Task Manager") | center | bold, separator(),
                 table_content->Render() | flex, separator(),
                 text(BOTTOM_TEXT) | center}) |
           border;
  });

  // define kill proc. menu
  ui::ModalOptions modal_options{};

  set_modal_entries(modal_options);

  auto exit = screen.ExitLoopClosure();

  Component final_ui = layout;
  auto modal_component = ui::ModalComponent(modal_options);

  final_ui |= Modal(modal_component, &modal_shown);

  // keybinds
  final_ui = CatchEvent(final_ui, [&](Event event) {
    return keybinds::handle_events(
        event, table_info, processes, process_manager, TOTAL_COLS, modal_shown,
        modal_options.show_modal, modal_options.hide_modal, screen);
  });

  auto trigger_update = [&screen]() { screen.PostEvent(Event::Custom); };

  app_is_running = true;
  ui::async_post_event_loop(trigger_update, UPDATE_INTERVAL_MS);

  screen.Loop(final_ui);

  app_is_running = false;
  return 0;
}

int get_pid_of_selected(int selected_row) {
  return processes[selected_row].pid;
}

void ui::async_post_event(Event event) {
  std::thread([event] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ScreenInteractive::Active()->PostEvent(event);
  }).detach();
}

void ui::async_post_event_loop(std::function<void()> post_event,
                               unsigned int interval_ms) {
  std::thread([post_event, interval_ms]() {
    while (app_is_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
      post_event();  // update
    }
  }).detach();
}

std::string calc_mem_to_str(long mem) {
  std::string mem_str;
  if (mem >= 1000000)
    return std::to_string(mem / 1000000) + " GB";
  else if (mem >= 1000)
    return std::to_string(mem / 1000) + " MB";

  return std::to_string(mem) + " KB";
}

ftxui::Component ui::create_table(std::vector<ProcessManager::Proc>& processes,
                                  ui::TableInfo& table_info) {
  using namespace std::literals;

  auto header_option = MenuOption::HorizontalAnimated();
  header_option.underline.color_active = Color::Plum2;
  header_option.underline.SetAnimationDuration(200ms);
  header_option.underline.SetAnimationFunction(animation::easing::BackOut);

  header_option.entries_option.transform = [](const EntryState& state) {
    int width = 0;
    switch (state.index) {
      case 0:
        width = 13;
        break;
      case 1:
        width = 30;
        break;
      case 2:
        width = 18;
        break;
      case 3:
        width = 18;
        break;
      case 4:
        width = 12;
        break;
    }
    Element e = text(state.label) | center;
    if (state.active) e = e | bold;
    return e | size(WIDTH, EQUAL, width);
  };

  auto header_menu =
      Menu(&header_entries, &table_info.selected_col, header_option);

  auto menu_option = MenuOption::Vertical();

  menu_option.on_change = [&]() {
    if (table_info.selected_row >= 0 &&
        table_info.selected_row < processes.size()) {
      table_info.tracked_pid = processes[table_info.selected_row].pid;
    }
  };

  menu_option.entries_option.transform = [&](const EntryState& state) {
    Element e = text(state.label) | borderEmpty;
    if (state.focused) e = e | bold;
    if (state.active) e = e | inverted;
    return e;
  };

  auto process_menu =
      Menu(&menu_entries, &table_info.selected_row, menu_option);

  auto container = Container::Vertical({header_menu, process_menu});
  container->SetActiveChild(process_menu);

  return Renderer(container, [&, header_menu, process_menu]() {
    static int last_sorted_col = -1;
    static bool last_sorted_asc = table_info.sorting_is_asc;

    static auto last_update_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    bool time_elapsed = (now - last_update_time) >=
                        std::chrono::milliseconds(UPDATE_INTERVAL_MS - 100);

    bool col_changed = (last_sorted_col != table_info.selected_col);
    bool asc_changed = (last_sorted_asc != table_info.sorting_is_asc);

    if (col_changed || asc_changed || time_elapsed || processes.empty()) {
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

      processes = process_manager.get_all_proc(table_info.sorting_method,
                                               table_info.sorting_is_asc);
      table_info.total_rows = processes.size();

      menu_entries.clear();
      for (const auto& p : processes) {
        std::stringstream ss;
        ss << " " << std::left << std::setw(12) << p.pid << std::left
           << std::setw(30) << p.name.substr(0, 29) << std::left
           << std::setw(18) << p.state << std::right << std::setw(17)
           << calc_mem_to_str(p.memory) << " " << std::right << std::setw(11)
           << (std::to_string(p.cpu_percent) + " %");
        menu_entries.push_back(ss.str());
      }

      if (table_info.tracked_pid != -1) {
        bool process_exists = false;
        for (int i = 0; i < processes.size(); ++i) {
          if (processes[i].pid == table_info.tracked_pid) {
            table_info.selected_row = i;
            process_exists = true;
            break;
          }
        }
        if (!process_exists) table_info.tracked_pid = -1;
      }

      last_sorted_col = table_info.selected_col;
      last_sorted_asc = table_info.sorting_is_asc;
      last_update_time = now;
    }

    return vbox({header_menu->Render(), separator(),
                 process_menu->Render() | vscroll_indicator | yframe | flex});
  });
}

ftxui::Component ui::ModalComponent(ui::ModalOptions options) {
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

void ui::set_modal_entries(ui::ModalOptions& modal_options) {
  modal_options.show_modal = [&] { modal_shown = true; };
  modal_options.hide_modal = [&] { modal_shown = false; };

  modal_options.sighup = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGHUP);
  };
  modal_options.sigint = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGINT);
  };
  modal_options.sigquit = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGQUIT);
  };
  modal_options.sigill = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGILL);
  };
  modal_options.sigtrap = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGTRAP);
  };
  modal_options.sigabrt = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGABRT);
  };
  modal_options.sigbus = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGBUS);
  };
  modal_options.sigfpe = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGFPE);
  };
  modal_options.sigkill = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGKILL);
  };
  modal_options.sigusr1 = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGUSR1);
  };
  modal_options.sigegv = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGSEGV);
  };
  modal_options.sigusr2 = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGUSR2);
  };
  modal_options.sigpipe = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGPIPE);
  };
  modal_options.sigalrm = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGALRM);
  };
  modal_options.sigterm = [&] {
    process_manager.kill_proc(get_pid_of_selected(table_info.selected_row),
                              SIGTERM);
  };
}