#ifndef DYNAMO_DEBUG_HPP
#define DYNAMO_DEBUG_HPP

#include <dynamo/dynamo.hpp>
#include <imgui.h>
#include <imgui-addons/imgui-addons.hpp>

namespace dynamo::gui{
    namespace component{
        struct GUI{
            bool show_window = false;
        };
    }

    class MainWindow{
    private:
        dynamo::Simulation& sim;

        ImGui::Addons::ScrollingPlot<float>     scrolling_plot_delta_time{"Delta time", 1000};
        ImGui::Addons::ScrollingPlot<int>       scrolling_plot_percepts{"Percepts", 1000};

        flecs::query<const tag::Agent, component::GUI> agents_query ;
        flecs::query<const tag::Artefact, component::GUI> artefacts_query ;
        flecs::query<const tag::Percept, component::GUI> percepts_query ;

        ImGuiTextFilter agents_list_filter;
        ImGuiTextFilter artefacts_list_filter;
        ImGuiTextFilter percepts_list_filter;

    public:
        bool is_enabled = false;

        explicit MainWindow(dynamo::Simulation& sim);

        void show();
    };
}

#endif //DYNAMO_DEBUG_HPP
