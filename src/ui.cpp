#include "ui.h"

#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for string, basic_string
#include <vector> // for vector

#include "ftxui/component/component.hpp"          // for Renderer, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                 // for Element, operator|, text, center, border

#include "../include/cpm.h"

using namespace ftxui;

auto ui_controller()
{
    return 0;
}

int ui::draw_ui()
{
    auto screen = ScreenInteractive::Fullscreen();

    // State:
    int left_size = 20;
    int right_size = 20;
    int top_size = 10;
    int bottom_size = 10;

    // Renderers:
    auto RendererInfo = [](const std::string &name, int *size)
    {
        return Renderer([name, size]
                        { return text(name + ": " + std::to_string(*size)) | center; });
    };
    auto middle = Renderer([]
                           { return text("Middle") | center; });
    auto left = RendererInfo("Left", &left_size);
    auto right = RendererInfo("Right", &right_size);
    auto top = RendererInfo("Top", &top_size);
    auto bottom = RendererInfo("Bottom", &bottom_size);

    auto container = middle;
    container = ResizableSplitLeft(left, container, &left_size);
    container = ResizableSplitRight(right, container, &right_size);
    container = ResizableSplitTop(top, container, &top_size);
    container = ResizableSplitBottom(bottom, container, &bottom_size);

    auto renderer =
        Renderer(container, [&]
                 { return container->Render() | border; });

    screen.Loop(renderer);
    return 0;
}


