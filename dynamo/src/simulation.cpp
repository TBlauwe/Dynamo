#include <dynamo/simulation.hpp>

dynamo::Simulation::Simulation() : Simulation(std::thread::hardware_concurrency() - 1) {}

dynamo::Simulation::Simulation(size_t number_of_threads) : executor{ number_of_threads } {
	_world.import<module::Core>();
	_world.import<module::GlobalPerception>();
	_world.import<module::BasicAction>();
	//_world.set<flecs::rest::Rest>({});
	_world.set<CommandsQueueHandle>({ &commands_queue });

	agents_query = _world.query<const dynamo::type::Agent>();
	flows = _world.system<Flow, Status, const Cyclic, const Launch>()
		.iter([this](flecs::iter& it, Flow* flow, Status* status, const Cyclic* cycle, const Launch* _)
		{
			for (auto i : it)
			{
				auto e = it.entity(i);
				status[i].value = executor.run(flow[i].taskflow);
				//status[i].value = executor.run(flow[i].taskflow,
				//	[id = e.id(), period = cycle[i].period, this]()
				//{
				//	commands_queue.push([id, period](flecs::world& world) mutable {
				//		flecs::entity(world, id).remove<Status>();
				//		flecs::entity(world, id).set<Cooldown>({ period });
				//		});
				//});
			}
		}
	);
}

void dynamo::Simulation::shutdown() {
	_world.each<Status>([](flecs::entity e, Status& status)
		{
			status.cancel();
		});
	executor.wait_for_all();
}

dynamo::Action dynamo::Simulation::action(const char* name) {
	return ActionBuilder(_world, name).build();
}

dynamo::Agent dynamo::Simulation::agent(const char* name) {
	return AgentBuilder(_world, name).build();
}

dynamo::Agent dynamo::Simulation::agent(AgentArchetype& archetype, const char* name) {
	auto agent = AgentBuilder(_world, name).build();
	agent.entity().is_a(archetype);
	return agent;
}

dynamo::AgentArchetype dynamo::Simulation::agent_archetype(const char* name) {
	return AgentArchetype(_world, name);
}

dynamo::AgentArchetype dynamo::Simulation::agent_archetype(AgentArchetype& archetype, const char* name) {
	return AgentArchetype(_world, archetype, name);
}

dynamo::Artefact dynamo::Simulation::artefact(const char* name) {
	return ArtefactBuilder(_world, name).build();
}

bool dynamo::Simulation::step(float elapsed_time) {
	bool should_quit = _world.progress(elapsed_time);

	flows.run();
	executor.wait_for_all();

	size_t size = commands_queue.size();    // Since size can be updated between loops (async),
											// we must check only once, not at every loop !
	for (int i = 0; i < size; i++)
	{
		auto command = commands_queue.pop();
		if (command && command.value()) // BUG : Somehow some commands are empty
			command.value()(_world);
	}

	return should_quit;
}

void dynamo::Simulation::step_n(unsigned int n, float elapsed_time) {
	for (int i = 0; i < n; i++) {
		step(elapsed_time);
	}
}

flecs::world& dynamo::Simulation::world() {
	return _world;
}