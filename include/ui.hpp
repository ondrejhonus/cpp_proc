#pragma once
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "manager.hpp"

class ui {
 public:
  ui() = default;
  ~ui() = default;

  int draw_ui();

  inline static ftxui::ButtonOption button_style =
      ftxui::ButtonOption::Animated();

  struct ModalOptions {
    const std::function<void()> sighup;
    const std::function<void()> sigint;
    const std::function<void()> sigquit;
    const std::function<void()> sigill;
    const std::function<void()> sigtrap;
    const std::function<void()> sigabrt;
    const std::function<void()> sigbus;
    const std::function<void()> sigfpe;
    const std::function<void()> sigkill;
    const std::function<void()> sigusr1;
    const std::function<void()> sigegv;
    const std::function<void()> sigusr2;
    const std::function<void()> sigpipe;
    const std::function<void()> sigalrm;
    const std::function<void()> sigterm;
    const std::function<void()> hide_modal;
  };

  struct TableInfo {
    int selected_row = 0;
    int selected_col = 0;
    int total_rows = 0;
    std::string sorting_method = "pid";
    bool sorting_is_asc = true;
  };
  static void async_post_event(ftxui::Event event);

 private:
  ftxui::Component create_table(std::vector<ProcessManager::Proc>& procs,
                                ui::TableInfo& table_info);
};
