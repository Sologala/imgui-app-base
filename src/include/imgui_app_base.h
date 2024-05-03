#ifndef __IMGUI_APP_HPP_
#define __IMGUI_APP_HPP_
#pragma once

#if WIN32
#include <windows.h>
#endif
// clang-format off
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <functional>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef ENABLE_PLOT
#include <implot.h>
#endif // ENABLE_PLOT

namespace ImGuiApp
{
class AppBase
{
  public:
    struct Option
    {
        std::string font_path;
        int font_size;
    };

    using DrawCallBack = std::function<bool()>;

  public:
    AppBase(const std::string &wnd_name, const ImVec2 &wnd_size, Option opt)
        : wnd_name(wnd_name), wnd_size(wnd_size), opt(opt)
    {
        BaseInit();
    };
    ~AppBase()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#if ENABLE_PLOT
        ImPlot::DestroyContext();
#endif // ENABLE_PLOT
        glfwTerminate();
    };

    void AddDrawCallBack(DrawCallBack cbk)
    {
        callbacks.emplace_back(cbk);
    }

    void Run()
    {
        bool cbk_return_false = false;
        while (!glfwWindowShouldClose(window) && !cbk_return_false)
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            {
                for (auto &cbk : callbacks)
                {
                    if (!cbk())
                    {
                        cbk_return_false = true;
                        printf("ImGui AppBase callback execute faild");
                        break;
                    }
                }
            }

            // We specify a default position/size in case there's no data in the .ini file.
            // We only do it to make the demo applications a little more welcoming, but typically this isn't required.

            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

  protected:
    bool BaseInit();

  private:
    void SetFontSize();

  protected:
    GLFWwindow *window;
    ImGuiContext *context;
    std::string wnd_name;
    ImVec2 wnd_size;
    Option opt;
    bool init_success = false;

    // draw callbacks
    std::vector<DrawCallBack> callbacks;
};
} // namespace ImGuiApp

#endif
