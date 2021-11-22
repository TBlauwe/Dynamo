#include <dynamo/gui/widgets/component_widgets.hpp>
#include <imgui.h>
#include <imgui-addons/imgui-addons.hpp>
#include <dynamo/gui/core.hpp>
#include <dynamo/modules/basic_perception.hpp>
#include <dynamo/modules/basic_stress.hpp>

namespace dynamo::widgets {

    template<>
    void show<type::Stress>(flecs::entity& e) {
        auto* stress = e.get_mut<type::Stress>();
        ImGui::Widgets::InputFloatColor(stress->value);
    }

    template<>
    void show<type::Decay>(flecs::entity& e){
        auto* decay = e.get_mut<type::Decay>();
        ImGui::Widgets::InputFloatColor(decay->ttl);
    }

    template<>
    void show<type::Cooldown>(flecs::entity& e){
        auto* cooldown = e.get_mut<type::Cooldown>(flecs::Wildcard);
        ImGui::Widgets::InputFloatColor(cooldown->remaining_time);
    }

    template<>
    void show<type::PeriodicEmitter>(flecs::entity& e){
        auto* emitter = e.get_mut<type::PeriodicEmitter>();
        ImGui::Widgets::InputFloatColor(emitter->cooldown);
    }

    template<>
    void show<type::Targets>(flecs::entity& e){
        auto* targets = e.get_mut<type::Targets>();
        ImGui::Text("Targets : ");
        ImGui::Indent();
        for(auto& entity : targets->entities){
            ImGui::BulletText("%s", entity.name().c_str());
        }
        ImGui::Unindent();
    }

    template<>
    void show<type::GUI>(flecs::entity& e){
        auto* gui = e.get_mut<type::GUI>();
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
                if(id == world.id<type::Decay>()) {
                    show<type::Decay>(entity);
                }else if(id == world.id<type::PeriodicEmitter>()) {
                    show<type::PeriodicEmitter>(entity);
                }else if (id == world.id<type::Stress>()) {
                    show<type::Stress>(entity);
                }else if(id == world.id<type::Targets>()) {
                    show<type::Targets>(entity);
                }else if(id == world.id<type::GUI>()) {
                    show<type::GUI>(entity);
                }else{
                    inspect(object);
                }
                break;
            case ID_TYPE::RELATION:{
                flecs::entity relation = id.relation();
                if(relation.id() == world.id<type::Cooldown>()) {
                    show<type::Cooldown>(entity);
                }else{
                    inspect(relation);
                    inspect(object);
                }
                break;
            }
            case ID_TYPE::CHILD_OF:
            case ID_TYPE::IS_A:
                inspect(object);
                break;
            case ID_TYPE::OWNED:
                ImGui::Text("See child instance");
                break;
            default:
                break;
        }
    }
}
