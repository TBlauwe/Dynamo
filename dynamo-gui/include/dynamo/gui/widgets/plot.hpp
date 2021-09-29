#ifndef DYNAMO_PLOT_HPP
#define DYNAMO_PLOT_HPP

#include <imgui-addons/imgui-addons.hpp>
#include <flecs.h>

namespace dynamo_gui::widget {
    template<typename TData, typename TEventType>
    class EventScrollingPlot : public ImGui::Addons::ScrollingPlot<TData> {
    private:
        flecs::query<const dynamo::type::Event, const TEventType> query ;

    public:
        EventScrollingPlot(flecs::world &world, const char *title, int capacity) :
            ImGui::Addons::ScrollingPlot<TData>(title, capacity),
            query{world.query<const dynamo::type::Event, const TEventType>()}
            {}

        void add(TData data) {
            this->push({data});
        };
    };
}

#endif //DYNAMO_PLOT_HPP
