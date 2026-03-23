#include <iostream>

#include "manager.hpp"
#include "ui.hpp"

#define UPDATE_INTERVAL_MS 3000

int main(int, char**) {
  ui UI;
  UI.async_post_event_loop(ftxui::Event::Custom, UPDATE_INTERVAL_MS);
  UI.draw_ui();
  return 0;
}
