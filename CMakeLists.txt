cmake_minimum_required(VERSION 3.12)

project(imgui_app_base)

include(./cmake/imgui-gl.cmake)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE imgui)

target_include_directories(${PROJECT_NAME} INTERFACE src)
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/include>
)

option(BUILD_TEST "build imgui_app_base test" OFF)

if (BUILD_TEST)
    add_subdirectory(test)
endif()
