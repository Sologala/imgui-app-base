#pragma once

#include <iostream>
#if WIN32
#include <windows.h>
#endif
// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <functional>
#include <glm/glm.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <string>
#include <unordered_map>
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
        int         font_size;
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

    void AddDrawCallBack(DrawCallBack cbk, bool after = false)
    {
        if (after)
            callbacksAfter.emplace_back(cbk);
        else
            callbacks.emplace_back(cbk);
    }

    void Run()
    {
        bool cbk_return_false = false;
        bool firsttime        = true;
        while (!glfwWindowShouldClose(window) && !cbk_return_false)
        {
            /* Render here */
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

            // check if resized
            static ImVec2 LastSize = ImGui::GetMainViewport()->Size;
            const ImVec2 &CurSize  = ImGui::GetMainViewport()->Size;
            if (LastSize.x != CurSize.x || LastSize.y != CurSize.y)
            {
                LastSize  = CurSize;
                firsttime = true;
            }

            if (firsttime)
            {
                firsttime = false;
                for (auto &cbk : callbacksBeforeDrawingOnce)
                {
                    if (!cbk())
                    {
                        cbk_return_false = true;
                        printf("ImGui AppBase before drawing callback execute faild");
                        break;
                    }
                }
            }

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

            // We specify a default position/size in case there's no data in the .ini
            // file. We only do it to make the demo applications a little more
            // welcoming, but typically this isn't required.

            ImGui::Render();
            {
                for (auto &cbk : callbacksAfter)
                {
                    if (!cbk())
                    {
                        cbk_return_false = true;
                        printf("ImGui AppBase callback execute faild");
                        break;
                    }
                }
            }

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    void AddBeforeStartDrawing(DrawCallBack cbk)
    {
        callbacksBeforeDrawingOnce.emplace_back(cbk);
    }
    void RegistLayoutBegin();

    void RegistLayout(const std::string &parentName, const std::string &childName1, const std::string &childName2,
                      const ImGuiDir dir, const float distRatio);
    void RegistLayoutEnd();

    ImGuiID GetLayoutID(const std::string &name)
    {
        if (layoutIds_.count(name) == 0)
            return 0;
        return layoutIds_[name];
    }

  protected:
    bool BaseInit();

  private:
    void SetFontSize();

  protected:
    GLFWwindow   *window;
    ImGuiContext *context;
    std::string   wnd_name;
    ImVec2        wnd_size;
    Option        opt;
    bool          init_success = false;

    // draw callbacks
    std::vector<DrawCallBack> callbacks;
    std::vector<DrawCallBack> callbacksAfter;
    std::vector<DrawCallBack> callbacksBeforeDrawingOnce;

    // layout regist
    std::unordered_map<std::string, ImGuiID> layoutIds_;
};
} // namespace ImGuiApp
