#include <dynamo/gui/widgets/entity_widgets.hpp>

#include <imgui.h>

void list_components(flecs::entity& entity){
    if (ImGui::TreeNode("Types")) {
        entity.each([](flecs::id &id) {
            if (id.is_pair()) {
                ImGui::Text("( %s - %s )", id.relation().name().c_str(), id.object().name().c_str());
            } else {
                ImGui::Text("( %s )", id.object().name().c_str());
            }
        });
        ImGui::TreePop();
    }
}

namespace dynamo::gui::widgets {
    void show_agent_widget(bool* open, flecs::entity& entity){
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, open)){
            ImGui::End();
            return;
        }
        list_components(entity);
        ImGui::End();
    }

    void show_artefact_widget(bool* open, flecs::entity& entity){
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, open)){
            ImGui::End();
            return;
        }
        list_components(entity);
        ImGui::End();
    }

    void show_percept_widget(bool* open, flecs::entity& entity){
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin("Percept", open)){
            ImGui::End();
            return;
        }
        list_components(entity);
        ImGui::End();
    }

    void show_organisation_widget(bool* open, flecs::entity& entity){
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, open)){
            ImGui::End();
            return;
        }
        ImGui::Text("Bonjour");
        ImGui::End();
    }

    void show_action_widget(bool* open, flecs::entity& entity){
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, open)){
            ImGui::End();
            return;
        }
        ImGui::Text("Bonjour");
        ImGui::End();
    }
}
