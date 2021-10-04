#ifndef DYNAMO_COMPONENT_WIDGETS_HPP
#define DYNAMO_COMPONENT_WIDGETS_HPP

#include <dynamo/modules/core.hpp>
#include <imgui.h>

namespace dynamo_gui::widget {

    template<typename T>
    void show(flecs::entity& e);


    enum class ID_TYPE{
        TAG,
        COMPONENT,
        RELATION,
        IS_A,
        CHILD_OF,
        OWNED,
        SKIP
    };

    ID_TYPE type_of(flecs::id& id);
    void inspect(flecs::entity& entity);
    void show_component_widget(flecs::entity& entity, ID_TYPE type, flecs::id& id);
}

#endif //DYNAMO_COMPONENT_WIDGETS_HPP
