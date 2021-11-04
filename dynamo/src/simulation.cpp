#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();

    agents_query = _world.query<dynamo::type::Agent>();
}

dynamo::Agent dynamo::Simulation::agent(const char * name) {
    return AgentBuilder(_world, name).build();
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