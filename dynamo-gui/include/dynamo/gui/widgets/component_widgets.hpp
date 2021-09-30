#ifndef DYNAMO_COMPONENT_WIDGETS_HPP
#define DYNAMO_COMPONENT_WIDGETS_HPP

#include <flecs.h>

namespace dynamo_gui::widget {
    enum class ID_TYPE{
        COMPONENT,
        RELATION,
        IS_A,
        CHILD_OF,
        SKIP
    };

    ID_TYPE type_of(flecs::id& id);
    void inspect(const flecs::entity& entity);
    void show_component_widget(ID_TYPE type, const flecs::id& id);
}

#endif //DYNAMO_COMPONENT_WIDGETS_HPP
