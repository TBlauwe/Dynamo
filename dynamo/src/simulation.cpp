#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() : Simulation(std::thread::hardware_concurrency()) {}

dynamo::Simulation::Simulation(size_t number_of_threads) : executor{ number_of_threads } {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();
    _world.import<module::BasicAction>();
    //_world.set<flecs::rest::Rest>({});

    agents_query = _world.query<const dynamo::type::Agent>();
    _world.set<type::CommandsQueueHandle>({&commands_queue});
}

void dynamo::Simulation::shutdown() {
    for_each([](flecs::entity e, const type::Agent _)
        {
            Agent(e).cancel_all_reasonning();
        });
    executor.wait_for_all();
}

dynamo::Action dynamo::Simulation::action(const char * name) {
    return ActionBuilder(_world, name).build();
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

    size_t size = commands_queue.size();    // Since size can be updated between for loop (async),
                                            // we must check only once, not at every loop !
    for (int i = 0; i<size; i++)
    {
        auto command = commands_queue.pop();
        if(command)
            command.value()(_world);
    }
}

void dynamo::Simulation::step_n(unsigned int n, float elapsed_time) {
    for (int i = 0; i < n; i++) {
        step(elapsed_time);
    }
}

flecs::world &dynamo::Simulation::world() {
    return _world;
}