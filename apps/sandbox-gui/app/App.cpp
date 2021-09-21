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

    static std::string event = "Some event";
    static ImGui::Addons::ScrollingPlot<int> plot{"Perceptions", 1000};
    bool    added {false};

    ImGui::InputText("Event", &event);
    if(rand()%100==1){
        auto e = sim.world.entity();
        e.set<dynamo::DecayingPercept>({2.0f});
        e.set<int>(rand()%100);
        if(rand()%1000==1){
            plot.add_point({true, event, sim.world.count<const dynamo::DecayingPercept>()});
            added = true;
        }
    }

    if(!added)
        plot.add_point({false, "", sim.world.count<const dynamo::DecayingPercept>()});

    sim.world.each([&](flecs::entity e, const dynamo::Agent& agent) {
    });
    plot.plot();

    ImGui::End();
}

