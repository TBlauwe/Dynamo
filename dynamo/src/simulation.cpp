#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() : Simulation(std::thread::hardware_concurrency()) {}

dynamo::Simulation::Simulation(size_t number_of_threads) : executor{ number_of_threads } {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();
    _world.set<flecs::rest::Rest>({});

    agents_query = _world.query<dynamo::type::Agent>();
    _world.set<type::CommandsQueueHandle>({&commands_queue});
}

void dynamo::Simulation::shutdown() {
    for_each([](flecs::entity e, type::Agent& _)
        {
            Agent(e).cancel_all_reasonning();
        });
    executor.wait_for_all();
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

    /** Performance critical section
    --------------------------------
    1. First method : flush_commands_queue();
    Flush all delayed commands
    Pros :
        * Ensure that all commands send by reasonner are applied as soon as possible and in the same iteration
    Cons :
        * Reasonners have an heavy impact on main loop performance
    Since we flush commands queue until they are no more commands, reasonners can still append commands to it (which delay even more the main loop)
    
    2. Second method : pop_commands_queue();
    Pop commands one by one for each tick, if there is one:

    Pros :
        * Extremely faster. Main loop can run as fast as possible
    Cons :
        * Only pop one command per tick. Not a problem if the amount of commands a low, may be if it is high
            * However, maybe with the gain in speed, this is not really a problem ?
    
    Overall, I think it is better to stick with the second method (for my current application at least). Maybe offer the choice ? Or define/find better alternatives ?
    */
    flush_for_commands_queue();
}

void dynamo::Simulation::pop_commands_queue() {
    auto command = commands_queue.pop();
    if (command.has_value())
        command.value()(_world);
}

void dynamo::Simulation::flush_commands_queue() {
    while (auto command = commands_queue.pop())
    {
        command.value()(_world);
    }
}

void dynamo::Simulation::flush_for_commands_queue() {
    size_t size = commands_queue.size();
    for (int i = 0; i<size; i++)
    {
        auto command = commands_queue.pop();
        if(command && command.value())
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