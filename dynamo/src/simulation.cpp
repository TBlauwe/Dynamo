#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();

    agents_query = _world.query<dynamo::type::Agent>();

    // ========== Trigger ==========

    _world.observer<component::AgentModel>()
            .event(flecs::OnSet)
            .each([this](flecs::entity e, component::AgentModel& model){
                e.set<component::Reasoner>({executor.run(model.taskflow)});
                e.remove<component::AgentModel>();
            });
}

dynamo::TypeHandler<dynamo::type::Agent> dynamo::Simulation::agent(const char * name) {
    return TypeBuilder<dynamo::type::Agent>(_world).create(name);
}

dynamo::TypeHandler<dynamo::type::Artefact> dynamo::Simulation::artefact(const char *name) {
    return TypeBuilder<dynamo::type::Artefact>(_world).create(name);
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
