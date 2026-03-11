cmake_minimum_required(VERSION 3.10.0)
project(cpm VERSION 0.1.0 LANGUAGES C CXX)


include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

add_executable(cpm 
    src/main.cpp 
    src/manager.cpp 
    src/ui.cpp
    src/sort.cpp
    src/sysinfo.cpp
)

target_include_directories(cpm PRIVATE include)

target_link_libraries(cpm PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
)