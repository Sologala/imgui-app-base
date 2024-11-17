#include "imapp/font.h"
#include <imapp/imapp.h>
#include <iostream>
#include <stdexcept>
namespace ImGuiApp
{

bool AppBase::BaseInit()
{
    auto glfw_error_callback = [](int error_code, const char *description) {
        printf("[GLFW] err code %x, err description %s\n", error_code, description);
    };
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        throw std::runtime_error("Glfw init faild");
    }
    glfwWindowHint(GLFW_SAMPLES, 4);

    if (wnd_size.x < 1.f && wnd_size.y < 1.f)
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        wnd_size.x *= mode->width;
        wnd_size.y *= mode->height;
    }
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(wnd_size.x, wnd_size.y, wnd_name.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Windows create faild");
    }
    glfwMakeContextCurrent(window);
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    context = ImGui::CreateContext();
#ifdef ENABLE_PLOT
    ImPlot::CreateContext();
#endif // ENABLE_PLOT
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    init_success = true;

    SetFontSize();

    return true;
}

void AppBase::SetFontSize()
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
        ImGuiIO     &io = ImGui::GetIO();
        ImFontConfig font_cfg;

        font_cfg.SizePixels           = opt.font_size;
        font_cfg.FontDataOwnedByAtlas = false;
        ImFont *font = io.Fonts->AddFontFromMemoryTTF(&Ubuntu_Regular, sizeof(Ubuntu_Regular), 32.0f, &font_cfg,
                                                      io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    }
    return;
}
} // namespace ImGuiApp
