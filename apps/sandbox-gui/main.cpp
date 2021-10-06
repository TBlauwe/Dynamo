#include <application/application.h>
#include <dynamo/dynamo.hpp>
#include <dynamo/gui/flecs_inspector.hpp>
#include <dynamo/gui/dynamo_inspector.hpp>

class Sandbox : public app::Application{

private:
    flecs::world world {};
    FlecsInspector flecs_inspector {world};
    dynamo_gui::DynamoInspector dynamo_inspector {world};

    bool    is_enabled  {false};
    bool    step_once   {false};
    float   timescale   {1.0f};
    float   min_timescale   {.05f};
    float   max_timescale   {10.0f};

public:
    Sandbox() :
        Application(1280,720,"Sandbox", "Sandbox")
    {
        auto& sim = dynamo::module_ref<dynamo::Simulation>(world.import<dynamo::Simulation>());

        auto core = world.get<dynamo::module::Core>();

        int n = 10;
        for(int i = 0; i<n; i++){
            world.entity(fmt::format("Agent {}", i).c_str())
                .is_a(core->Agent);
        }

        std::vector<flecs::entity_view> agents {};
        core->agents_query.each([&agents](flecs::entity agent, dynamo::type::Agent& _){
            agents.emplace_back(agent);
        });

        world.entity("Radio")
            .is_a(core->Artefact)
            .set<dynamo::component::PeriodicEmitter>({0.5f})
            .set<dynamo::component::Targets>({agents});
    }

private:
    void on_menu_bar() override{
        ImGui::Separator();
        if(is_enabled){
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.3f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.3f, 0.8f, 0.8f));
        }else{
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.1f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.1f, 0.8f, 0.8f));
            if(ImGui::Button("  " ICON_FA_STEP_FORWARD " Step  ")){
                step_once = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
        }
        if(ImGui::Button(!is_enabled ? "  " ICON_FA_PAUSE " Paused  " : "  " ICON_FA_PLAY " Playing  ")){
            is_enabled = !is_enabled;
        }
        ImGui::PopStyleColor(3);

        ImGui::Separator();
        ImGui::SetNextItemWidth(150.f);
        if(ImGui::SliderFloat("TimeScale", &timescale, min_timescale, max_timescale)){
            world.set_time_scale(timescale);
        }
        ImGui::Addons::HelpMarker("Ctrl + Click to input a specific value");
        ImGui::Separator();
    }

    void on_update() override{
        flecs_inspector.show();
        dynamo_inspector.show();
        if(is_enabled || step_once){
            world.progress(ImGui::GetIO().DeltaTime);
            step_once = false;
        }
    }
};

int main(){
    Sandbox sandbox;
    sandbox.run();
}