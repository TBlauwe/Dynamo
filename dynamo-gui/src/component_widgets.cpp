#include <dynamo/gui/widgets/component_widgets.hpp>
#include <imgui.h>

namespace dynamo_gui::widget {
    ID_TYPE type_of(flecs::id& id){
        if(id.has_relation(flecs::IsA))
            return ID_TYPE::IS_A;
        if(id.has_relation(flecs::ChildOf))
            return ID_TYPE::CHILD_OF;
        if(id.is_pair())
            if(id.relation().name().operator==("Identifier"))
                return ID_TYPE::SKIP;
            else
                return ID_TYPE::RELATION;
        else
            return ID_TYPE::COMPONENT;
    }

    void show_component_widget(const ID_TYPE type, const flecs::id& id){
        switch (type) {
            case ID_TYPE::COMPONENT:
            case ID_TYPE::RELATION:
                break;
            case ID_TYPE::CHILD_OF:
            case ID_TYPE::IS_A:
                inspect(id.object());
                break;
            default:
                break;
        }
    }
}
