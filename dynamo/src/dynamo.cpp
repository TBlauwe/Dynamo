//
// Created by Tristan on 09/09/2021.
//
#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation()
{
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%10n] %^(%8l)%$ %v");
    logger->info("Dynamo launching ...");
}

flecs::entity dynamo::Simulation::add_agent(const char *name) const{
    auto e = world.entity(name);
    e.add<tag::Agent>();
    return e;
}

flecs::entity dynamo::Simulation::add_artefact(const char * name) const{
    auto e = world.entity(name);
    e.add<tag::Artefact>();
    return e;
}

void dynamo::Simulation::add_event(const char * name) const{
    world.set<component::Event>({name});
}

template<typename TPerceptType, typename TData>
void dynamo::Simulation::add_percept(TPerceptType percept_type, TData data){
    perception.add_percept(percept_type, data);
}

void dynamo::Simulation::run() const{
    world.progress();
}

void dynamo::Simulation::shutdown() const{
    logger->info("Dynamo shutting down ...");
}