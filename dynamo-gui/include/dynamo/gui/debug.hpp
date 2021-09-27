#ifndef DYNAMO_DEBUG_HPP
#define DYNAMO_DEBUG_HPP

#include <dynamo/dynamo.hpp>
#include <imgui.h>
#include <imgui-addons/imgui-addons.hpp>
#include <dynamo/gui/widgets/plot.hpp>

namespace dynamo::gui{
    namespace component{
        struct GUI{
            bool show_window = false;
        };
    }

    class MainWindow{
    private:
        dynamo::Simulation& sim;

        widgets::EventScrollingPlot<float, ::dynamo::event::MAJOR>     scrolling_plot_delta_time{sim.world, "Delta time", 1000};
        widgets::EventScrollingPlot<int, ::dynamo::event::MINOR>       scrolling_plot_percepts{sim.world, "Percepts", 1000};

        flecs::query<const tag::Agent, component::GUI> agents_query ;
        flecs::query<const tag::Artefact, component::GUI> artefacts_query ;
        flecs::query<const tag::Percept, component::GUI> percepts_query ;
        flecs::query<const tag::Organisation, component::GUI> organisations_query ;

        ImGuiTextFilter agents_list_filter;
        ImGuiTextFilter artefacts_list_filter;
        ImGuiTextFilter percepts_list_filter;
        ImGuiTextFilter organisations_list_filter;

        float timescale = 1.0f;

    public:
        bool is_enabled = false;

        explicit MainWindow(dynamo::Simulation& sim);

        void show();
    };
}

#endif //DYNAMO_DEBUG_HPP
