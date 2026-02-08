#include "ui.h"

#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for string, basic_string
#include <vector> // for vector
#include "ftxui/component/component.hpp"          // for Renderer, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                 // for Element, operator|, text, center, border
#include "ftxui/dom/table.hpp"

#include "../include/cpm.h"

using namespace ftxui;

auto ui_controller()
{
    return 0;
}

int ui::draw_ui()
{
    cpm manager;
    auto table_component = Renderer([&]() -> Element {
        auto procs = manager.get_all_proc();

        std::vector<std::vector<std::string>> table_data;
        
        // head
        table_data.push_back({"PID", "NAME", "STATE", "MEMORY"});

        // data
        for (const auto& p : procs) {
            table_data.push_back({
                std::to_string(p.pid),
                p.name,
                p.state,
                std::to_string(p.memory) + " KB"
        });
    }
    auto table = Table(table_data);
    
    table.SelectAll().Separator();
    table.SelectRow(0).Decorate(bold);
    
   return table.Render() | vscroll_indicator | frame | flex;
});


    // Sizes:
    auto screen = ScreenInteractive::Fullscreen();
    int top_size = 3;
    int head_size = 1;
    int bottom_size = 2;

    auto top = Renderer([&] { return text("CPM - Task Manager") | center; });
    auto header = Renderer([&] { return text("Sorting: [Name] [PID] [Mem]") | center; });
    auto bottom_status = Renderer([&] { return text("Press Q to Quit") | center; });

    // Renderers:
    auto container = Renderer([&] {
        return vbox({
            top->Render(),
            separator(),
            header->Render(),
            separator(),
            table_component->Render() | flex, 
            separator(),
            bottom_status->Render()
        });
    });

    auto main_renderer = Renderer(container, [&] {
        return container->Render() | border;
    });

    main_renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q')) {
        screen.Exit();
        return true;
    }
    return false;
});

    screen.Loop(main_renderer);
    return 0;
}


