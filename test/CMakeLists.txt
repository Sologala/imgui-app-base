cmake_minimum_required(VERSION 3.12)

add_executable(test_app ./test_app.cpp)
target_link_libraries(test_app imgui_app_base imgui)

if(ENABLE_PLOT)
  add_executable(implot_demo ./test_plot_demo.cpp
                             ../third_party/implot/implot_demo.cpp)
  target_link_libraries(implot_demo imgui_app_base imgui)
endif()
