#include "App.hpp"

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

    auto emitter = sim.add_artefact("Radio");
    emitter.set<dynamo::component::RandomIntEmitter>({0,5,1.0f,2.0f});

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
        sim.run();
    }
}

