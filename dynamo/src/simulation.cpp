#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();

    agents_query = _world.query<dynamo::type::Agent>();
    std::cout << " -- Simulation : Starting.\n";
}

void dynamo::Simulation::shutdown() {
    std::cout << " -- Simulation : Waiting for all threads to finished.\n";
    executor.wait_for_all();
    std::cout << " -- Simulation : Finished.\n";
}

dynamo::Agent dynamo::Simulation::agent(const char * name) {
    return AgentBuilder(_world, name).build();
}

dynamo::Agent dynamo::Simulation::agent(AgentArchetype& archetype, const char * name) {
    auto agent = AgentBuilder(_world, name).build();
    agent.entity().is_a(archetype);
    return agent;
}

dynamo::AgentArchetype dynamo::Simulation::agent_archetype(const char * name) {
    return AgentArchetype(_world, name);
}

dynamo::AgentArchetype dynamo::Simulation::agent_archetype(AgentArchetype& archetype, const char * name) {
    return AgentArchetype(_world, archetype, name);
}

dynamo::Artefact dynamo::Simulation::artefact(const char *name) {
    return ArtefactBuilder(_world, name).build();
}

void dynamo::Simulation::step(float elapsed_time) {
    _world.progress(elapsed_time);
}

void dynamo::Simulation::step_n(unsigned int n, float elapsed_time) {
    for (int i = 0; i < n; i++) {
        step(elapsed_time);
    }
}

flecs::world &dynamo::Simulation::world() {
    return _world;
}