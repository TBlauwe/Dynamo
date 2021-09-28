#ifndef DYNAMO_PLOT_HPP
#define DYNAMO_PLOT_HPP

#include <imgui-addons/imgui-addons.hpp>
#include <flecs.h>

namespace dynamo_gui::widget {
    template<typename TData, typename TEvent>
    class EventScrollingPlot : public ImGui::Addons::ScrollingPlot<TData> {
    private:
        flecs::world &world;

    public:
        EventScrollingPlot(flecs::world &world, const char *title, int capacity) :
                ImGui::Addons::ScrollingPlot<TData>(title, capacity),
                world{world}{
            world.system<const dynamo::component::Event<TEvent>>()
                    .kind(flecs::OnStore)
                    .each([](flecs::entity e, const dynamo::component::Event<TEvent> event){
                        e.remove<dynamo::component::Event<TEvent>>();
                    });
        }

        void add(TData data) {
            this->push({world.has<dynamo::component::Event<TEvent>>(),
                world.has<dynamo::component::Event<TEvent>>() ? world.get<dynamo::component::Event<TEvent>>()->name: "",
                data});
        };

    };
}

#endif //DYNAMO_PLOT_HPP
