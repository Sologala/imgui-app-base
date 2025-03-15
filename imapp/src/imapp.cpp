#include "imapp/font.h"
#include "imgui_internal.h"
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

void AppBase::RegistLayout(const std::string &parentName, const std::string &childName1, const std::string &childName2,
                           const ImGuiDir dir, const float distRatio)
{
    // get id
    ImGuiID pid;

    std::string pname = (parentName.empty() ? "root" : parentName);

    if (layoutIds_.count(pname) == 0)
    {
        std::cout << "wrong parent node Name " << pname << std::endl;
        return;
    }

    pid = layoutIds_[pname];
    ImGuiID cid1, cid2;
    ImGui::DockBuilderSplitNode(pid, dir, distRatio, &cid1, &cid2);
    ImGui::DockBuilderDockWindow(childName1.c_str(), cid1);
    ImGui::DockBuilderDockWindow(childName2.c_str(), cid2);
    layoutIds_[childName1] = cid1;
    layoutIds_[childName2] = cid2;
}

void AppBase::RegistLayoutBegin()
{
    constexpr ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiViewport *viewport = ImGui::GetMainViewport();

    // create root node and full screen
    ImGui::DockBuilderRemoveNode(main_win_id_);
    ImGui::DockBuilderAddNode(main_win_id_, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(main_win_id_, viewport->Size);

    layoutIds_["root"] = main_win_id_;
}

void AppBase::RegistLayoutEnd()
{
    ImGui::DockBuilderFinish(layoutIds_["root"]);
}
} // namespace ImGuiApp
