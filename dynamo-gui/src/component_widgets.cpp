#include <dynamo/gui/widgets/component_widgets.hpp>
#include <imgui.h>
#include <imgui-addons/imgui-addons.hpp>
#include <dynamo/modules/basic_perception.hpp>
#include <dynamo/gui/core.hpp>
#include <limits>

namespace dynamo_gui::widget {

    template<>
    void show<dynamo::component::Decay>(flecs::entity& e){
        auto* decay = e.get_mut<dynamo::component::Decay>();
        auto* initial_decay = e.get<dynamo::component::InitialValue<dynamo::component::Decay>>();
        ImGui::Addons::SliderFloatColor(decay->ttl, 0.f, initial_decay->memory.ttl);
    }

    template<>
    void show<dynamo::component::Cooldown>(flecs::entity& e){
        auto* cooldown = e.get_mut<dynamo::component::Cooldown>();
        auto* initial_cooldown = e.get<dynamo::component::InitialValue<dynamo::component::Cooldown>>();
        ImGui::Addons::SliderFloatColor(cooldown->value, 0.f, initial_cooldown->memory.value);
    }

    template<>
    void show<dynamo::component::InitialValue<dynamo::component::Decay>>(flecs::entity& e){
        auto* initial_cooldown = e.get_mut<dynamo::component::InitialValue<dynamo::component::Decay>>();
        ImGui::SliderFloat("Initial TTL", &initial_cooldown->memory.ttl, 0.f, 3600.f);
    }

    template<>
    void show<dynamo::component::InitialValue<dynamo::component::Cooldown>>(flecs::entity& e){
        auto* initial_cooldown = e.get_mut<dynamo::component::InitialValue<dynamo::component::Cooldown>>();
        ImGui::SliderFloat("Refresh rate", &initial_cooldown->memory.value, 0.f, 3600.f);
    }

    template<>
    void show<dynamo_gui::component::GUI>(flecs::entity& e){
        auto* gui = e.get_mut<dynamo_gui::component::GUI>();
        ImGui::Checkbox("Show GUI", &gui->show_widget);
    }

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
        else{
            flecs::entity entity = id.entity();
            if(entity.is_valid()){
                if(id.has_role(flecs::Owned)){
                    return ID_TYPE::OWNED;
                }else if(entity.has<EcsComponent>()){
                    return entity.get<EcsComponent>()->size > 0 ? ID_TYPE::COMPONENT : ID_TYPE::TAG;
                }else{
                    return ID_TYPE::SKIP;
                }
            }else{
                return ID_TYPE::SKIP;
            }
        }
    }

    void show_component_widget(flecs::entity& entity, ID_TYPE type, flecs::id& id){
        flecs::entity object = id.object();
        flecs::world world = id.world();
        switch (type) {
            case ID_TYPE::COMPONENT:
                if(id == world.id<dynamo::component::Decay>()) {
                    show<dynamo::component::Decay>(entity);
                }else if(id == world.id<dynamo::component::Cooldown>()) {
                    show<dynamo::component::Cooldown>(entity);
                }else if(id == world.id<dynamo::component::InitialValue<dynamo::component::Cooldown>>()) {
                    show<dynamo::component::InitialValue<dynamo::component::Cooldown>>(entity);
                }else if(id == world.id<dynamo::component::InitialValue<dynamo::component::Decay>>()) {
                    show<dynamo::component::InitialValue<dynamo::component::Decay>>(entity);
                }else if(id == world.id<dynamo_gui::component::GUI>()) {
                    show<dynamo_gui::component::GUI>(entity);
                }else{
                    inspect(object);
                }
                break;
            case ID_TYPE::RELATION:
            case ID_TYPE::CHILD_OF:
            case ID_TYPE::IS_A:
                inspect(object);
                break;
            case ID_TYPE::TAG:
                ImGui::Text("Tag have no associated data");
                break;
            case ID_TYPE::OWNED:
                ImGui::Text("See child instance");
                break;
            default:
                break;
        }
    }
}