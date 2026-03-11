#include "keybinds.hpp"

#include "ui.hpp"

using namespace ftxui;

bool keybinds::handle_events(ftxui::Event event, ui::TableInfo& table_info,
                             int total_cols, bool& modal_shown,
                             std::function<void()> show_modal,
                             std::function<void()> hide_modal,
                             ftxui::ScreenInteractive& screen) {
  if (event == Event::Character('q') || event == Event::Escape) {
    if (modal_shown) {
      modal_shown = false;
      return true;
    }
    screen.Exit();
    return true;
  }

  if (event == Event::ArrowUp || event == Event::k ||
      (event.is_mouse() && event.mouse().button == Mouse::WheelUp)) {
    if (table_info.selected_row > 0 && !modal_shown) {
      table_info.selected_row--;
      return true;
    }
    return false;
  }
  if (event == Event::l || event == Event::ArrowRight) {
    if (table_info.selected_col < total_cols - 1) {
      table_info.selected_col++;
      ui::async_post_event(Event::Custom);
      return true;
    }
    return false;
  }
  if (event == Event::h || event == Event::ArrowLeft) {
    if (table_info.selected_col > 0) {
      table_info.selected_col--;
      ui::async_post_event(Event::Custom);
      return true;
    }
    return false;
  }
  if (event == Event::ArrowDown || event == Event::j ||
      (event.is_mouse() && event.mouse().button == Mouse::WheelDown)) {
    if (table_info.selected_row < table_info.total_rows - 1 && !modal_shown) {
      table_info.selected_row++;
      return true;
    }
    // default behav.
    return false;
  }

  if (event == Event::K) {
    modal_shown == true ? hide_modal() : show_modal();
    return true;
  }
  if (event == Event::PageDown) {
    table_info.selected_row = table_info.total_rows;
    return true;
  }
  if (event == Event::PageUp) {
    table_info.selected_row = 1;
    return true;
  }
  if ((event == Event::Return || event == Event::Character(' ')) &&
      !modal_shown) {
    table_info.sorting_is_asc = !table_info.sorting_is_asc;
    screen.PostEvent(Event::Custom);
    return true;
  }

  return false;
}