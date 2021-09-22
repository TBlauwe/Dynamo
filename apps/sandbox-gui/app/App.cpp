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
    sim.add_agent("Arthur");
    sim.add_agent("Bruno");
}

void Sandbox::on_update() {
    debug.show();
    if(debug.is_enabled){
        if(rand()%10==1){
            auto entity_percept = sim.world.entity();
            entity_percept.add<dynamo::tag::Percept>();
            entity_percept.set<dynamo::component::DecayingPercept>({2.0f});
            entity_percept.set<int>(rand()%100);
        }

        sim.run();
    }
}

