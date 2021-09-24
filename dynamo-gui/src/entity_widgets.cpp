#include <dynamo/gui/widgets/entity_widgets.hpp>

#include <imgui.h>

namespace dynamo::gui::widgets {
    void show_agent_widget(bool* open, flecs::entity& entity){
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, open)){
            ImGui::End();
            return;
        }
        ImGui::Text("Bonjour");
        ImGui::End();
    }

    void show_artefact_widget(bool* open, flecs::entity& entity){
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
