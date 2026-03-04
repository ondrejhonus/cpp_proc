cmake_minimum_required(VERSION 3.10.0)
project(cpp_proc VERSION 0.1.0 LANGUAGES C CXX)


include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

add_executable(cpp_proc 
    src/main.cpp 
    src/cpm.cpp 
    src/ui.cpp
    src/sort.cpp
)

target_include_directories(cpp_proc PRIVATE include)

target_link_libraries(cpp_proc PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
)