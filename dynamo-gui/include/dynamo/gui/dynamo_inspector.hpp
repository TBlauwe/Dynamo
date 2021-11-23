#ifndef DYNAMO_DYNAMO_INSPECTOR_HPP
#define DYNAMO_DYNAMO_INSPECTOR_HPP

#include <dynamo/simulation.hpp>
#include <dynamo/gui/core.hpp>
#include <dynamo/gui/widgets/plot.hpp>

namespace dynamo{
    class DynamoInspector{
    private:
        Simulation&     sim;
        flecs::world&   world;

        widgets::EventScrollingPlot<size_t, float> scrolling_plot_percepts{world, "Percepts", 1000};

        flecs::query<const type::Action,        type::GUI> actions_query ;
        flecs::query<const type::Agent,         type::GUI> agents_query ;
        flecs::query<const type::Artefact,      type::GUI> artefacts_query ;
        flecs::query<const type::Organisation,  type::GUI> organisations_query ;
        flecs::query<const type::Percept,       type::GUI> percepts_query ;

        ImGuiTextFilter actions_list_filter;
        ImGuiTextFilter agents_list_filter;
        ImGuiTextFilter artefacts_list_filter;
        ImGuiTextFilter organisations_list_filter;
        ImGuiTextFilter percepts_list_filter;

    public:
        explicit DynamoInspector(Simulation&);

        void show();

    private:
        void show_actions_panel();
        void show_agents_panel();
        void show_artefacts_panel();
        void show_organisations_panel();
        void show_percepts_panel();
        void show_node_editor();
    };
}

#endif //DYNAMO_DYNAMO_INSPECTOR_HPP
