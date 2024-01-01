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
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Demonstrate the various window flags. Typically you would just use the default!
            static bool no_titlebar = false;
            static bool no_scrollbar = false;
            static bool no_menu = false;
            static bool no_move = false;
            static bool no_resize = false;
            static bool no_collapse = false;
            static bool no_close = false;
            static bool no_nav = false;
            static bool no_background = false;
            static bool no_bring_to_front = false;
            static bool unsaved_document = false;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoCollapse;
            bool is_open = true;
            ImGui::Begin("Panel", &is_open, window_flags);
            {
                for (auto &cbk : callbacks)
                {
                    cbk();
                }
            }
            ImGui::End();

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
        if (opt.font_path.empty() == 0 || opt.font_size)
        {
            return;
        }

        ImGuiIO &io = ImGui::GetIO();
        ImFontConfig font_cfg;
        // TODO: implementing font loading

        font_cfg.SizePixels = opt.font_size;
        auto dft_font = io.Fonts->AddFontDefault(&font_cfg);
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
