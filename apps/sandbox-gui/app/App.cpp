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
    sim.add_agent("Agent");
    sim.add_agent("Bruno");
    sim.world.system<dynamo::tag::Agent>("Reasoner")
            .interval(1)
            .each([this](flecs::entity entity_agent, const dynamo::tag::Agent& agent){
                int sum = 0;
                entity_agent.each<dynamo::relation::sees>([&sum](flecs::entity entity_percept){
                    sum += entity_percept.get<dynamo::component::Integer>()->value;
                }) ;
                sim.logger->info("{} is counting {}", entity_agent.name(), sum);
            });
}

void Sandbox::on_update() {
    debug.show();
    if(debug.is_enabled){
        if(rand()%10==1){
            sim.add_percept<dynamo::component::DecayingPercept, dynamo::component::Integer>({2.0f}, {rand()%100});
        }
        sim.run();
    }
}

