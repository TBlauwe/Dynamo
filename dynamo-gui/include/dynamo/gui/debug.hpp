#ifndef DYNAMO_DEBUG_HPP
#define DYNAMO_DEBUG_HPP

#include <dynamo/dynamo.hpp>
#include <dynamo/gui/core.hpp>
#include <dynamo/gui/widgets/plot.hpp>

namespace dynamo_gui{
    class Inspector{
    private:
        dynamo::Simulation& sim;

        dynamo_gui::widget::EventScrollingPlot<float, ::dynamo::event::MAJOR>     scrolling_plot_delta_time{sim.world, "Delta time", 1000};
        dynamo_gui::widget::EventScrollingPlot<int, ::dynamo::event::MINOR>       scrolling_plot_percepts{sim.world, "Percepts", 1000};

        flecs::query<const dynamo::tag::Agent, component::GUI> agents_query ;
        flecs::query<const dynamo::tag::Artefact, component::GUI> artefacts_query ;
        flecs::query<const dynamo::tag::Percept, component::GUI> percepts_query ;
        flecs::query<const dynamo::tag::Organisation, component::GUI> organisations_query ;
        flecs::query<const dynamo::tag::Action, component::GUI> actions_query ;

        ImGuiTextFilter agents_list_filter;
        ImGuiTextFilter artefacts_list_filter;
        ImGuiTextFilter percepts_list_filter;
        ImGuiTextFilter organisations_list_filter;
        ImGuiTextFilter actions_list_filter;

        float timescale = 1.0f;

    public:
        bool is_enabled = false;

        explicit Inspector(dynamo::Simulation& sim);

        void show();
    };
}

#endif //DYNAMO_DEBUG_HPP
