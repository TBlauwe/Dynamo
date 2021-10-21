#ifndef DYNAMO_FLECS_INSPECTOR_HPP
#define DYNAMO_FLECS_INSPECTOR_HPP

#include <flecs.h>
#include <imgui-addons/imgui-addons.hpp>
#include <set>

class FlecsInspector {
private:
    flecs::world& world;

    ImGui::Widgets::ScrollingPlot<float> scrolling_plot_fps {"FPS", 1000};
    ImGui::Widgets::ScrollingPlot<float> scrolling_plot_delta_time {"Delta time (renderer)", 1000};
    ImGui::Widgets::ScrollingPlot<float> scrolling_plot_world_delta_time {"Delta time (world)", 1000};

    size_t table_count {0};
    size_t entities_count {0};
    ImGui::Widgets::ScrollingPlot<int> scrolling_plot_entities {"Entities", 1000};

    ImGuiTextFilter type_list_filter;
    ImGuiTextFilter components_list_filter;
    ImGuiTextFilter systems_list_filter;

    flecs::query<>  systems_query {world.query_builder<>()
            .term(flecs::System)
            .build()
    };

    std::set<uint64_t> disabled_features {};

public:
    explicit FlecsInspector(flecs::world& world);

    void show();

private:
    void update();
    void show_overall_panel();
    void show_features_panel();
    void show_components_panel();
    void show_systems_panel();

};


#endif //DYNAMO_FLECS_INSPECTOR_HPP
