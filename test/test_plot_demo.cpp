#include <chrono>
#include <imgui_app_base.h>
#include <implot.h>
#include <math.h>
#if defined(WIN32)
// #include <windowsx.h>
#endif
#include <algorithm>

struct ScrollingBuffer
{
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000)
    {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y)
    {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else
        {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase()
    {
        if (Data.size() > 0)
        {
            Data.shrink(0);
            Offset = 0;
        }
    }
};
int main(int argc, char *argv[])
{
    ImGuiApp::AppBase::Option opt;
    opt.font_size = 30;
    ImGuiApp::AppBase app("test-app", ImVec2(800, 600), opt);
    bool clicked = false;
    app.AddDrawCallBack([clicked]() {
        ImPlot::ShowDemoWindow();
        ImGui::Begin("app", nullptr);
        static auto lst_ts = std::chrono::system_clock::now();
        auto cur_ts = std::chrono::system_clock::now();
        size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(cur_ts - lst_ts).count();
        lst_ts = cur_ts;
        static ScrollingBuffer dataAnalog;
        static float t = 0;
        dataAnalog.AddPoint(t, 1000.f / std::fmax(static_cast<double>(diff), 0.0001));

        t += ImGui::GetIO().DeltaTime;
        static float rratios[] = {3, 1};
        static float cratios[] = {5, 1, 1};

        if (ImPlot::BeginSubplots("##Digital", 2, 3, ImVec2(-1, 300), 0, rratios, cratios))
        {

            if (ImPlot::BeginPlot("",ImVec2(),ImPlotFlags_NoLegend)) {
                ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0, t, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

                ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1));
                if (dataAnalog.Data.size() > 0)
                {
                    ImPlot::PlotLine("fps", &dataAnalog.Data[0].x, &dataAnalog.Data[0].y, dataAnalog.Data.size(), 0,
                                     dataAnalog.Offset, 2 * sizeof(float));
                }
                ImPlot::EndPlot();
            }

            ImPlot::EndSubplots();
        }

        /* ImPlot::PlotDigitalG("fps",  */
        ImGui::End();
        return true;
    });

    /* app.AddDrawCallBack([]() { return false; }); // Cbk return false will terminate whole application */
    app.Run();

    return 0;
}
