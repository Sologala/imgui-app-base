#include <imapp/canvas/canvas.h>
#include <imapp/imapp.h>
#include <imapp/primitives/cylinder.h>
#include <imapp/primitives/mesh.h>

#if defined(WIN32)
#include <windowsx.h>
#endif

int main(int argc, char *argv[])
{
    ImGuiApp::AppBase::Option opt;
    opt.font_size = 30;
    ImGuiApp::AppBase app("test-app", ImVec2(1600, 900), opt);

    bool clicked = false;

    ImGuiCanvas canvas("viewport");

    Cylinder::Sptr cylinder = std::make_shared<Cylinder>();

    Mesh::Sptr mesh = std::make_shared<Mesh>("../resource/sedan.stl");
    mesh->SetAlpha(0.85f);
    canvas.AddDrawable("mesh", mesh, true);
    // canvas.AddDrawable("cylinder", cylinder);

    app.AddBeforeStartDrawing([&]() {
        app.RegistLayoutBegin();
        app.RegistLayout("", "viewport", "app", ImGuiDir_Left, 0.5);
        app.RegistLayoutEnd();
        return true;
    });

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
        canvas.Draw();
        return true;
    });

    app.AddDrawCallBack([&]() {
        ImGui::SetNextWindowDockID(app.GetLayoutID("viewport"), ImGuiCond_Always);
        ImGui::Begin("test");
        ImGui::Text("hhh");
        ImGui::End();
        ImGui::SetNextWindowDockID(app.GetLayoutID("app"), ImGuiCond_Once);
        ImGui::ShowDemoWindow();
        return true;
    });

    // load stl

    /* app.AddDrawCallBack([]() { return false; }); // Cbk return false will terminate whole application */
    app.Run();

    return 0;
}
