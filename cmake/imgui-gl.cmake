cmake_minimum_required(VERSION 3.12)

message("START Prepare Imgui Library")

# headers and paths
set(CPP_INCLUDE_DIRS "")
set(CPP_SOURCE_FILES "")
set(CPP_HEADER_FILES "")

# This section deal with the character sets UNICODE _UNICODE
# Directx win32 settings
# this deal with convert string
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# control where the static and shared libraries are built so that on windows
# we don't need to tinker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(ENABLE_IMGUI ON)

if(ENABLE_IMGUI)
  if (NOT imgui_FOUND) # If there's none, fetch and build sdl
    include(FetchContent)
    FetchContent_Declare(
      imgui
      GIT_REPOSITORY https://github.com/ocornut/imgui.git
      # GIT_TAG v1.82
    )
    if (NOT imgui_POPULATED) # Have we downloaded yet?
      set(FETCHCONTENT_QUIET NO)
      FetchContent_MakeAvailable(imgui)
	  #INCLUDE
      list(APPEND CPP_INCLUDE_DIRS ${imgui_SOURCE_DIR})
      list(APPEND CPP_INCLUDE_DIRS ${imgui_SOURCE_DIR}/backends)
	  #SOURCE
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_demo.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_draw.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_tables.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
	  #HEADER
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/imgui.h)
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/imconfig.h)
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/imgui_internal.h)
    endif()
  endif()
endif()


    FetchContent_Declare(
      glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG 3.3.9
    )
    FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
# Fetch the content using previously declared details
    FetchContent_Populate(glfw)

# Set custom variables
    set(BUILD_SHARED_LIBS OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)
    set(GLFW_VULKAN_STATIC OFF)

    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

# Bring the populated content into the build
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

#opengl add backend
message("set IMGUI backend opengl + glfw")
list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h)
list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h)

add_library(imgui
  ${CPP_HEADER_FILES}
  ${CPP_SOURCE_FILES}
)

find_package( OpenGL 3.2 REQUIRED )
find_package(GLEW)
if (NOT GLEW_FOUND)
    message("GLEW NOT Found")

    FetchContent_Declare(
      glew
      GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
      # GIT_TAG glew-cmake-2.2.0
    )
    FetchContent_GetProperties(glew)
    if(NOT glew_POPULATED)
# Fetch the content using previously declared details
        FetchContent_Populate(glew)
        add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR})
    endif()
    target_link_libraries(imgui PUBLIC libglew_static)
else()
    target_link_libraries(imgui PUBLIC GLEW::GLEW)
endif()

target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARIES} glfw)
# target_include_directories(imgui PUBLIC ${GLEW_INCLUDE_DIRS})

message("CMAKE_CURRENT_SOURCE_DIR  >>> " ${CMAKE_CURRENT_SOURCE_DIR})
message("IMGUI_INCLUDE_DIRS  >>> " ${CPP_INCLUDE_DIRS})
# target_include_directories(imgui PUBLIC
#     ${CPP_INCLUDE_DIRS}
# )
target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
    $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CPP_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:imgui>
)
