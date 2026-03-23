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

  typedef struct {
    std::function<void()> sighup;
    std::function<void()> sigint;
    std::function<void()> sigquit;
    std::function<void()> sigill;
    std::function<void()> sigtrap;
    std::function<void()> sigabrt;
    std::function<void()> sigbus;
    std::function<void()> sigfpe;
    std::function<void()> sigkill;
    std::function<void()> sigusr1;
    std::function<void()> sigegv;
    std::function<void()> sigusr2;
    std::function<void()> sigpipe;
    std::function<void()> sigalrm;
    std::function<void()> sigterm;
    std::function<void()> show_modal;
    std::function<void()> hide_modal;
  } ModalOptions;

  typedef struct {
    int selected_row = 0;
    int selected_col = 4;  // Default sorting method: CPU
    int tracked_pid = -1;
    int total_rows = 0;
    std::string sorting_method;
    bool sorting_is_asc = false;
  } TableInfo;

  static void async_post_event(ftxui::Event event);
  static void async_post_event_loop(ftxui::Event, unsigned int interval_ms);

 private:
  ftxui::Component create_table(std::vector<ProcessManager::Proc>& processes,
                                ui::TableInfo& table_info);
  ftxui::Component ModalComponent(ui::ModalOptions);
  void set_modal_entries(ui::ModalOptions& modal_options);

  std::vector<std::string> menu_entries;
  std::vector<std::string> header_entries = {"[PID]", "[NAME]", "[STATE]",
                                             "[MEMORY]", "[CPU%]"};
  ftxui::Component process_menu;
};
