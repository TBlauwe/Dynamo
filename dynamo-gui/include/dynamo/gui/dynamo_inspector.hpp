#ifndef DYNAMO_DYNAMO_INSPECTOR_HPP
#define DYNAMO_DYNAMO_INSPECTOR_HPP

#include <dynamo/dynamo.hpp>
#include <dynamo/gui/core.hpp>
#include <dynamo/gui/widgets/plot.hpp>

namespace dynamo_gui{
    class Inspector{
    private:
        flecs::world& world;

        dynamo_gui::widget::EventScrollingPlot<int, float>       scrolling_plot_percepts{world, "Percepts", 1000};

        flecs::query<const dynamo::type::Agent, component::GUI> agents_query ;
        flecs::query<const dynamo::type::Artefact, component::GUI> artefacts_query ;
        flecs::query<const dynamo::type::Percept, component::GUI> percepts_query ;
        flecs::query<const dynamo::type::Organisation, component::GUI> organisations_query ;
        flecs::query<const dynamo::type::Action, component::GUI> actions_query ;

        ImGuiTextFilter agents_list_filter;
        ImGuiTextFilter artefacts_list_filter;
        ImGuiTextFilter percepts_list_filter;
        ImGuiTextFilter organisations_list_filter;
        ImGuiTextFilter actions_list_filter;

    public:
        explicit Inspector(flecs::world& world);

        void show();
    };
}

#endif //DYNAMO_DYNAMO_INSPECTOR_HPP
