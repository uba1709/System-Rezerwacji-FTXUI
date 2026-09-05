cmake_minimum_required(VERSION 3.20)

project(SystemRezerwacjiFTXUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v6.1.9
)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ftxui)

add_executable(system_rezerwacji main.cpp)
target_link_libraries(system_rezerwacji PRIVATE
  ftxui::component
  ftxui::dom
  ftxui::screen
)