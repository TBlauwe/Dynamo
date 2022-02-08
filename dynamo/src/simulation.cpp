#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() : Simulation(std::thread::hardware_concurrency()) {}

dynamo::Simulation::Simulation(size_t number_of_threads) : executor{ number_of_threads } {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();
    _world.import<module::BasicAction>();
    //_world.set<flecs::rest::Rest>({});

    agents_query = _world.query<const dynamo::type::Agent>();
    _world.set<CommandsQueueHandle>({&commands_queue});
}

void dynamo::Simulation::shutdown() {
    for_each([](flecs::entity e, const type::Agent _)
        {
            if (e.is_alive())
                Agent(e).cancel_all_reasonning();
            else
                std::cout << "Agent " << e.name() << "died unexpectedly ! \n";
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

bool dynamo::Simulation::step(float elapsed_time) {
    bool should_quit = _world.progress(elapsed_time);

    size_t size = commands_queue.size();    // Since size can be updated between for loop (async),
                                            // we must check only once, not at every loop !
    for (int i = 0; i<size; i++)
    {
        auto command = commands_queue.pop();
        if(command && command.value()) // BUG : Somehow some commands are empty
            command.value()(_world);
    }

    return should_quit;
}

void dynamo::Simulation::step_n(unsigned int n, float elapsed_time) {
    for (int i = 0; i < n; i++) {
        step(elapsed_time);
    }
}

flecs::world &dynamo::Simulation::world() {
    return _world;
}