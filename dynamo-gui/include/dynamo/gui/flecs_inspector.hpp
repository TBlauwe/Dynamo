#ifndef DYNAMO_FLECS_INSPECTOR_HPP
#define DYNAMO_FLECS_INSPECTOR_HPP

#include <flecs.h>
#include <imgui-addons/imgui-addons.hpp>

class FlecsInspector {
private:
    flecs::world& world;

    ImGui::Addons::ScrollingPlot<float> scrolling_plot_fps {"FPS", 1000};
    ImGui::Addons::ScrollingPlot<float> scrolling_plot_delta_time {"Delta time (renderer)", 1000};
    ImGui::Addons::ScrollingPlot<float> scrolling_plot_world_delta_time {"Delta time (world)", 1000};
    ImGui::Addons::ScrollingPlot<float> scrolling_plot_system_delta_time {"Delta time (system)", 1000};

    size_t table_count {0};
    size_t entities_count {0};
    ImGui::Addons::ScrollingPlot<float> scrolling_plot_entities {"Entities", 1000};

public:
    explicit FlecsInspector(flecs::world& world);

    void show();

private:
    void update();
};


#endif //DYNAMO_FLECS_INSPECTOR_HPP
