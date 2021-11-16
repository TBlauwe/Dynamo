#ifndef DYNAMO_PLOT_HPP
#define DYNAMO_PLOT_HPP

#include <imgui-addons/imgui-addons.hpp>
#include <flecs.h>

//TODO Find better solution for annotating plot
namespace dynamo::widgets {
    template<typename TData, typename TEventType>
    class EventScrollingPlot : public ImGui::Widgets::ScrollingPlot<TData> {
    private:
        //flecs::query<const dynamo::type::Event, const TEventType> query ;

    public:
        EventScrollingPlot(flecs::world &world, const char *title, int capacity) :
            ImGui::Widgets::ScrollingPlot<TData>(title, capacity)
            //query{world.query<const dynamo::type::Event, const TEventType>()}
            {}

        void add(TData data) {
            this->push({data});
        };
    };
}

#endif //DYNAMO_PLOT_HPP
