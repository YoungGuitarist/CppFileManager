cmake_minimum_required(VERSION 3.10)
project(CppFileManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ftxui REQUIRED)

add_executable(app main.cpp)

target_link_libraries(app 
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)


