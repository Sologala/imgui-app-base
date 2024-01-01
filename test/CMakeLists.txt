cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
add_executable(test_app ./test_app.cpp)
target_link_libraries(test_app  imgui_app)
