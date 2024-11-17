#include <imgui_app_base.h>
#if defined(WIN32)
#include <windowsx.h>
#endif

#include "canvas.h"

int main(int argc, char *argv[])
{
    ImGuiApp::AppBase::Option opt;
    opt.font_size = 30;
    ImGuiApp::AppBase app("test-app", ImVec2(800, 600), opt);

    bool clicked = false;

    Canvas canvas(800, 600);

    app.AddDrawCallBack([clicked]() {
        ImGui::Begin("app", nullptr);
        /* ImGui::SetWindowSize(ImVec2(800, 600)); */
        /* ImGui::SetWindowPos(ImVec2(0, 0)); */

        bool clicked = ImGui::Button("Click Me", ImVec2(100, 100));
        if (clicked)
        {
            ImGui::Text("clicked");
        }
        ImGui::Text("Static Text");
        ImGui::End();
        return true;
    });
    app.AddDrawCallBack([&]() {
        ImGui::Begin("view_port", nullptr);
        const float window_width  = ImGui::GetContentRegionAvail().x;
        const float window_height = ImGui::GetContentRegionAvail().y;

        canvas.ScaleTo(window_width, window_height);

        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGui::GetWindowDrawList()->AddImage(canvas.GetTextureId(), ImVec2(pos.x, pos.y),
                                             ImVec2(pos.x + window_width, pos.y + window_height), ImVec2(0, 1),
                                             ImVec2(1, 0));

        ImGui::End();
        return true;
    });

    app.AddDrawCallBack(
        [&]() {
            static int cnt = 0;
            canvas.cam.m_x_rot += ((cnt / 100) % 2 == 1 ? 1 : -1) * 0.5;
            canvas.cam.m_z_rot += ((cnt / 100) % 2 == 1 ? 1 : -1) * 0.5;
            canvas.Rend();
            return true;
        },
        true);

    /* app.AddDrawCallBack([]() { return false; }); // Cbk return false will terminate whole application */
    app.Run();

    return 0;
}
