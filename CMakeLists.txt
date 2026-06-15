cmake_minimum_required(VERSION 3.20)

project(DemoSceneLab)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(DemoSceneLab
    src/main.cpp
)

target_include_directories(DemoSceneLab PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(DemoSceneLab PRIVATE ${SDL2_LIBRARIES})