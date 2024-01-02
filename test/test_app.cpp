#include <imgui_app_base.h>
#include <windowsx.h>


int main(int argc, char *argv[])
{
    ImGuiApp::AppBase::Option opt;
    opt.font_size = 30;
    ImGuiApp::AppBase app("test-app", ImVec2(800, 600), opt);
    bool clicked = false;
    app.AddDrawCallBack([clicked]() {
        ImGui::Begin("app", nullptr);
        ImGui::SetWindowSize(ImVec2(800, 600));
        ImGui::SetWindowPos(ImVec2(0, 0));

        bool clicked = ImGui::Button("Click Me", ImVec2(100, 100));
        if (clicked)
        {
            ImGui::Text("clicked");
        }
        ImGui::Text("Static Text");
        ImGui::End();
        return true;
    });

    /* app.AddDrawCallBack([]() { return false; }); // Cbk return false will terminate whole application */
    app.Run();

    return 0;
}
