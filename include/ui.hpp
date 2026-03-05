#pragma once
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "ftxui/component/captured_mouse.hpp"

class ui
{
public:
    ui() = default;
    ~ui() = default;

    int draw_ui();

private:
    bool handle_events(ftxui::Event event, int& selected_row, int& selected_col, 
        int total_rows, int total_cols, bool& modal_shown, std::function<void()> show_modal, 
        std::function<void()> hide_modal, ftxui::ScreenInteractive& screen);

    void async_post_event(ftxui::Event event);
};