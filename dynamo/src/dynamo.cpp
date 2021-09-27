//
// Created by Tristan on 09/09/2021.
//
#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation() {
    world.import<module::Perception>();
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

void dynamo::Simulation::run() const{
    world.progress();
}