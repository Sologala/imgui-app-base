cmake_minimum_required(VERSION 3.12)

project(imgui_app_base)

include(./cmake/imgui-gl.cmake)

option(ENABLE_PLOT "ENABLE_IMPLOT" ON)
option(BUILD_TEST "build imgui_app_base test" OFF)

set(CMAKE_CXX_STANDARD 11)

set(APP_INCLUDE_DIRS "")
set(APP_SOURCE_FILES "")

list(APPEND APP_INCLUDE_DIRS ./src/include)
list(APPEND APP_SOURCE_FILES ./src/imgui_app_base.cpp)

if(ENABLE_PLOT)
  add_definitions(-DENABLE_PLOT)
  list(APPEND APP_INCLUDE_DIRS ./third_party/implot)
  list(APPEND APP_SOURCE_FILES ./third_party/implot/implot_items.cpp)
  list(APPEND APP_SOURCE_FILES ./third_party/implot/implot.cpp)
endif()

find_package(Eigen3 REQUIRED)

add_library(${PROJECT_NAME} ${APP_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC imgui Eigen3::Eigen)
target_include_directories(${PROJECT_NAME}
                           PUBLIC $<BUILD_INTERFACE:${APP_INCLUDE_DIRS}>)

if (MSVC)
add_definitions(-DNOMINMAX)
endif()

if(BUILD_TEST)
  message("BUILD TEST")
  add_subdirectory(test)
endif()
