#include "App.hpp"
#include <string>
#include <boost/circular_buffer.hpp>
#include <imgui-addons/imgui-addons.hpp>

namespace app
{
    void on_initialization(){
    }

    void on_shutdown(){
    }
    namespace ImGuiWindows{
        void menu_bar(){
        }
    }
}

Sandbox::Sandbox() :
        Application(1280,720,"Sandbox", "Sandbox"),
        sim(),
        number_perception(sim.world)
{
    auto arthur = sim.add_agent("Arthur");
    logger->info("Arthur id : {}", arthur);
    auto  bruno = sim.add_agent("Bruno");
    logger->info("Bruno id : {}", bruno);
}

void Sandbox::on_update() {
    sim.run();
    ImGui::Begin("Dynamo");

    static int number {0};
    static int entity {0};
    static std::string event;
    static ImGui::Addons::ScrollingPlot<int> plot{"Perceptions", 1000};
    static bool    send_event {false};
    bool    added {false};

    ImGui::DragInt("Number", &number);
    ImGui::DragInt("Entity", &entity);
    ImGui::InputText("Event", &event);
    ImGui::Checkbox("Send with event ?", &send_event);
    ImGui::SameLine();
    if(ImGui::Button("Send")){
        auto e = sim.world.entity();
        e.set<dynamo::DecayingPercept>({2.0f});
        e.set<int>(2);
        if(send_event){
            plot.add_point({true, event, sim.world.count<const dynamo::DecayingPercept>()});
            added = true;
        }
    }

    if(!added)
        plot.add_point({false, "", sim.world.count<const dynamo::DecayingPercept>()});

    sim.world.each([](flecs::entity e, const dynamo::Agent& agent) {
        ImGui::Text("[%llu] %s", e.id(), e.name().c_str());
    });
    plot.plot();

    ImGui::End();
}

