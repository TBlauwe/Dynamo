#include <dynamo/gui/flecs_inspector.hpp>
#include <imgui.h>
#include <iostream>

FlecsInspector::FlecsInspector(flecs::world &world) : world{world} {}

void FlecsInspector::update() {
    scrolling_plot_delta_time.push({ImGui::GetIO().DeltaTime});
    scrolling_plot_world_delta_time.push({world.delta_time()});
    //scrolling_plot_system_delta_time.push({world.de()});
    scrolling_plot_fps.push({ImGui::GetIO().Framerate});
    table_count = 0;
    entities_count = 0;
    float delta = 0;
    for (auto it : world) {
        delta += it.delta_system_time();
        //flecs::type table_type = it.table_type();
        table_count += 1;
        entities_count += it.count();
        //std::cout << it.type().str() << std::endl;
    }
    scrolling_plot_system_delta_time.push({delta});
}

void FlecsInspector::show() {
    update();

    ImGui::Begin("Flecs");
    if (ImGui::BeginTabBar("FlecsTabBar")) {
        if (ImGui::BeginTabItem("Overall")) {
            ImPlot::SetNextPlotLimits(0, scrolling_plot_delta_time.capacity(), 0, 0.5, ImGuiCond_Once);
            if (ImPlot::BeginPlot("Rendering statistics",
                                  nullptr,
                                  "seconds",
                                  {-1, 0},
                                  ImPlotFlags_YAxis2,
                                  ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoGridLines,
                                  ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoGridLines,
                                  ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoGridLines,
                                  ImPlotAxisFlags_None,
                                  "fps"
            )) {
                scrolling_plot_delta_time.plot();
                scrolling_plot_world_delta_time.plot();
                scrolling_plot_system_delta_time.plot();
                ImPlot::SetPlotYAxis(ImPlotYAxis_2);
                scrolling_plot_fps.plot();
                ImPlot::EndPlot();
            }

            if (ImGui::BeginTable("World statistics##table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Variable", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Table");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", table_count);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Entities");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", entities_count);
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

