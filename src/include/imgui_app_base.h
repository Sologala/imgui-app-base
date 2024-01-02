#ifndef __IMGUI_APP_HPP_
#define __IMGUI_APP_HPP_
#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

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
    bool BaseInit()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Glfw init faild");
        }

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(wnd_size.x, wnd_size.y, wnd_name.c_str(), NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Windows create faild");
        }
        glfwMakeContextCurrent(window);
        context = ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        init_success = true;

        SetFontSize();

        return true;
    }

  private:
    void SetFontSize()
    {
        if (!init_success)
        {
            return;
        }
        if (opt.font_path.empty() && opt.font_size == 0)
        {
            return;
        }
        else if (opt.font_path.empty() && opt.font_size)
        {
            ImGuiIO &io = ImGui::GetIO();
            ImFontConfig font_cfg;
            font_cfg.SizePixels = opt.font_size;
            auto dft_font = io.Fonts->AddFontDefault(&font_cfg);
        }
        else if (opt.font_path.empty() && opt.font_size)
        {
            // TODO implementing of font loading from file
        }
        return;
    }

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
