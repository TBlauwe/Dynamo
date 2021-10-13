#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();
}

flecs::entity dynamo::Simulation::agent(const char * name) {
    auto core = _world.get<module::Core>();
    return _world.entity()
        .is_a(core->Agent)
        ;
}

flecs::entity dynamo::Simulation::artefact(const char *name) {
    auto core = _world.get<module::Core>();
    return _world.entity()
            .is_a(core->Artefact)
            ;
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
