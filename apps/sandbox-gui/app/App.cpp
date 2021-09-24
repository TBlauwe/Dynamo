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
    int n = 10;
    for(int i = 0; i<n; i++){
        auto e = sim.add_agent(fmt::format("Agent {}", i).c_str());
        int min = rand()%10;
        int max = min + rand()%11;
        e.set<dynamo::component::IntSensor>({min,max});
    }
    //auto e = sim.add_agent("Agent");
    //e.set<dynamo::component::IntSensor>({5,10});
    //e = sim.add_agent("Bruno");
    //e.set<dynamo::component::IntSensor>({0,4});

    auto emitter = sim.add_artefact("Radio");
    emitter.set<dynamo::component::RandomIntEmitter>({0,11,1.0f,2.0f});

    sim.world.system<dynamo::tag::Agent>("Reasoner")
            .interval(1)
            .each([this](flecs::entity entity_agent, const dynamo::tag::Agent& agent){
                int sum = 0;
                entity_agent.each<dynamo::relation::sees>([&sum](flecs::entity entity_percept){
                    sum += entity_percept.get<dynamo::component::Integer>()->value;
                }) ;
                //sim.logger->info("{} is counting {}", entity_agent.name().c_str(), sum);
            });
}

void Sandbox::on_update() {
    debug.show();
    if(debug.is_enabled){
        sim.run();
    }
}

