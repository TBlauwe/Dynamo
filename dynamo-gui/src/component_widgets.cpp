#include <dynamo/gui/widgets/component_widgets.hpp>
#include <imgui.h>
#include <imgui_internal.h>

namespace dynamo_gui::widget {

    template<>
    void show<dynamo::component::Decay>(flecs::entity& e){
        auto* decay = e.get_mut<dynamo::component::Decay>();
        auto* initial_decay = e.get<dynamo::component::InitialValue<dynamo::component::Decay>>();
        float min = 0;
        float max = initial_decay->memory.ttl;
        float gradient = decay->ttl / (max * 3);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(gradient, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(gradient, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(gradient, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(gradient, 0.9f, 0.9f));
        ImGui::SliderFloat("", &(*decay).ttl, min, max, "%.3f");
        ImGui::PopStyleColor(4);
    }

    template<>
    void show<dynamo::component::InitialValue<dynamo::component::Decay>>(flecs::entity& e){
        show<dynamo::component::Decay>(e);
    };

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

    void show_component_widget(flecs::entity& entity, ID_TYPE type, flecs::id& id){
        flecs::entity e = id.object();
        switch (type) {
            case ID_TYPE::COMPONENT:
                if(e.name()=="Decay") {
                    show<dynamo::component::Decay>(entity);
                }else{
                    inspect(e);
                }
                break;
            case ID_TYPE::RELATION:
                break;
            case ID_TYPE::CHILD_OF:
            case ID_TYPE::IS_A:
                inspect(e);
                break;
            default:
                break;
        }
    }
}
