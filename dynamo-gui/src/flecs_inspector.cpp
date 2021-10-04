#include <dynamo/gui/flecs_inspector.hpp>
#include <imgui.h>
#include <spdlog/fmt/bundled/format.h>

FlecsInspector::FlecsInspector(flecs::world &world) : world{world} {}

void FlecsInspector::update() {
    scrolling_plot_delta_time.push({ImGui::GetIO().DeltaTime});
    scrolling_plot_world_delta_time.push({world.delta_time()});
    scrolling_plot_fps.push({ImGui::GetIO().Framerate});
    table_count = 0;
    entities_count = 0;
    float delta = 0;
    for (auto it : world) {
        delta += it.delta_system_time();
        table_count += 1;
        entities_count += it.count();
    }
    scrolling_plot_entities.push({static_cast<int>(entities_count)});
}

void FlecsInspector::show() {
    update();

    ImGui::Begin("Flecs");
    if (ImGui::BeginTabBar("FlecsTabBar")) {
        if (ImGui::BeginTabItem("Overall")) {
            show_overall_panel();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Entities")) {
            show_entities_panel();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Components")) {
            show_components_panel();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Systems")) {
            show_systems_panel();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void FlecsInspector::show_overall_panel() {
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
        ImPlot::SetPlotYAxis(ImPlotYAxis_2);
        scrolling_plot_fps.plot();
        ImPlot::EndPlot();
    }

    ImPlot::SetNextPlotLimits(0, scrolling_plot_entities.capacity(), 0, 100, ImGuiCond_Once);
    if (ImPlot::BeginPlot("World statistics",
                          nullptr,
                          "count",
                          {-1, 0},
                          ImPlotFlags_None,
                          ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoGridLines,
                          ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoGridLines
    )) {
        scrolling_plot_entities.plot();
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
}

void FlecsInspector::show_entities_panel() {
    entities_list_filter.Draw();
    ImGui::Spacing();
    if (ImGui::BeginTable("Entities##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        world.each<>([this](flecs::entity e) {
            ImGui::PushID(static_cast<int>(e.id()));
            if (entities_list_filter.PassFilter(fmt::format("{}", e.name()).c_str())) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%llu", e.id());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", e.name().c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", "...");

                ImGui::TableSetColumnIndex(3);
            }
            ImGui::PopID();
        });
        ImGui::EndTable();
    }
}

void FlecsInspector::show_components_panel() {
    components_list_filter.Draw();
    ImGui::Spacing();
    if (ImGui::BeginTable("Components##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Alignment", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        world.each<EcsComponent>([this](flecs::entity e, EcsComponent& component) {
            ImGui::PushID(static_cast<int>(e.id()));
            {
                if (components_list_filter.PassFilter(fmt::format("{}", e.name()).c_str())) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV((component.size > 0 ?  0.60f : 0.30f), 0.6f, 0.6f))));
                    ImGui::Text("%llu", e.id());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", e.name().c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", component.size);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d", component.alignment);
                }
            }
            ImGui::PopID();
        });
        ImGui::EndTable();
    }
}
void FlecsInspector::show_systems_panel() {
    systems_list_filter.Draw();
    ImGui::Spacing();
    if (ImGui::BeginTable("Systems##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Alignment", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        systems_query.each([this](flecs::entity e) {
            ImGui::PushID(static_cast<int>(e.id()));
            {
                if (systems_list_filter.PassFilter(fmt::format("{}", e.name()).c_str())) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%llu", e.id());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", e.name().c_str());
                }
            }
            ImGui::PopID();
        });
        ImGui::EndTable();
    }
}
