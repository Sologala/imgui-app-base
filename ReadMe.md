# ImGui_AppBase

🚩 `ImGui_AppBase` wrapped the routine of `ImGui` and `OpenGL` as well as `Glfw`, which like GUI context initialization and destroying.
Let us be more concentrated on the main processes.

## Usage

```c++
#include <imgui.h>
#include <imgui_app_base.h>

int main(int argc, char *argv[])
{
    ImGuiApp::AppBase::Option opt;
    opt.font_size = 30;
    ImGuiApp::AppBase app("test-app", ImVec2(800, 600), opt);
    bool clicked = false;
    app.AddDrawCallBack([clicked]() {
        // Main processes is handled by AddDrawCallBack.
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

    app.Run();

    return 0;
}
```
