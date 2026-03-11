#pragma once

#include <ui.hpp>

#include "ftxui/component/component.hpp"

class keybinds {
 public:
  keybinds() = default;
  ~keybinds() = default;

 public:
  static bool handle_events(ftxui::Event event, ui::TableInfo& table_info,
                            int total_cols, bool& modal_shown,
                            std::function<void()> show_modal,
                            std::function<void()> hide_modal,
                            ftxui::ScreenInteractive& screen);
};
