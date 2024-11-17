# for glfw - has cmake buildchain
add_subdirectory(glfw)

# for glm - has cmake buildchain
add_subdirectory(glm)

# for imgui - header only get_filename_component(THIRD_PARTY_DIR
# ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(IMGUI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

list(APPEND IMGUI_PUBLIC_HEADERS "${IMGUI_SRC_DIR}/imgui.h")

list(
  APPEND
  IMGUI_PRIVATE_HEADERS
  "${IMGUI_SRC_DIR}/imgui_internal.h"
  "${IMGUI_SRC_DIR}/imconfig.h"
  "${IMGUI_SRC_DIR}/stb_rect_pack.h"
  "${IMGUI_SRC_DIR}/stb_textedit.h"
  "${IMGUI_SRC_DIR}/stb_truetype.h"
  "./imgui/misc/cpp/imgui_stdlib.h")

list(
  APPEND
  IMGUI_SOURCES
  "${IMGUI_SRC_DIR}/imgui.cpp"
  "${IMGUI_SRC_DIR}/imgui_draw.cpp"
  "${IMGUI_SRC_DIR}/imgui_demo.cpp"
  "${IMGUI_SRC_DIR}/imgui_widgets.cpp"
  "./imgui/imgui_tables.cpp"
  "${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp"
  "./imgui/backends/imgui_impl_glfw.cpp"
  "./imgui/misc/cpp/imgui_stdlib.cpp"
  "./glad/src/gl.c")

list(APPEND IMGUI_PUBLIC_INCLUDE_DIRS ${IMGUI_SRC_DIR})

list(APPEND IMGUI_PUBLIC_LIBRARIES glfw glm)

set(NAME imgui)
add_library(${NAME} STATIC ${IMGUI_SOURCES})
target_link_libraries(${NAME} PUBLIC ${IMGUI_PUBLIC_LIBRARIES})
target_include_directories(
  ${NAME}
  PUBLIC ${IMGUI_PUBLIC_INCLUDE_DIRS} ./glad/include/
  PRIVATE ${IMGUI_PRIVATE_INCLUDE_DIRS})
