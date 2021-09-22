#ifndef DYNAMO_DEBUG_HPP
#define DYNAMO_DEBUG_HPP

#include <imgui.h>
#include <dynamo/dynamo.hpp>
#include <imgui-addons/imgui-addons.hpp>

namespace dynamo::gui{
    class MainWindow{
    private:
        dynamo::Simulation& sim;

        ImGui::Addons::ScrollingPlot<float> scrolling_plot_delta_time{"Delta time", 1000};
        ImGui::Addons::ScrollingPlot<int> scrolling_plot_percepts{"Percepts", 1000};

    public:
        bool is_enabled = false;

        explicit MainWindow(dynamo::Simulation& sim);

        void show();
    };
}

#endif //DYNAMO_DEBUG_HPP
