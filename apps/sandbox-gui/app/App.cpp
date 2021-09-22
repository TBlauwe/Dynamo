#include "App.hpp"
#include <string>

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
        debug(sim)
{
    auto arthur = sim.add_agent("Arthur");
    sim.logger->info("Arthur id : {}", arthur);
    auto bruno = sim.add_agent("Bruno");
    sim.logger->info("Bruno id : {}", bruno);
}

void Sandbox::on_update() {
    if(debug.is_enabled){
        sim.run();
    }

    if(rand()%10==1){
        //sim.perception.add<dynamo::component::DecayingPercept, int>({2.0f}, rand()%100);
        auto entity_percept = sim.world.entity();
        entity_percept.add<dynamo::tag::Percept>();
        entity_percept.set<dynamo::component::DecayingPercept>({2.0f});
        entity_percept.set<int>(rand()%100);
        sim.world.each([&](flecs::entity entity_agent) {
            //entity_agent.add(sim.perception.sees, entity_percept);
        });
    }

    debug.show();
}

