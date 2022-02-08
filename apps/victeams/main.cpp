#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>
#include <dynamo/modules/basic_action.hpp>
#include <dynamo/modules/activity_dl.hpp>

using namespace dynamo;

struct act {};

class SimpleReasonner : public AgentModel
{
public:
	using AgentModel::AgentModel;

	virtual constexpr const char* name() const { return "MySuperReasonner"; }

	void build() override
	{
		auto perception = emplace([](AgentHandle agent)
			{
			}
		);
		perception.name("Perception");

		auto feasible = process<strat::ContainerAccumulator, std::vector<flecs::entity>>();
		feasible.name("FeasibleActions");
		feasible.succeed(perception);

		auto selection = process<strat::InfluenceGraph, flecs::entity, std::vector<flecs::entity>>(feasible);
		selection.name("TaskSelection");

		auto execution = process<strat::Sequential, flecs::entity>(selection);
		execution.name("Execution");
	}
};

struct Idle {};
struct Work {};

struct Patient {};
struct State
{
	struct Well {};
	struct Unwell {};
};

struct Type
{
	struct Communication {};
	struct Physical {};
	struct Attention {};
};

struct Result
{
	std::stringstream ss;
};

int main(int argc, char** argv) {

	// -----------------------------
	// SETUP
	// -----------------------------

	// -- Create an empty simulation
	Simulation sim{};
	sim.world().import<module::BasicStress>();
	sim.world().import<module::BasicAction>();
	sim.world().import<module::ADL>();

	sim.world().component<act>().add(flecs::Exclusive);

	sim.world().type<State>()
		.add<State::Well>()
        .add<State::Unwell>();

	auto root = sim.world().entity("Root").add<type::Root>();

	sim.action("Do nothing")
		.add<Idle>()
		.set<type::Cost>({ 0 });

	sim.action("Read a book")
		.add<Idle>()
		.set<type::Cost>({ 1 });

	sim.action("Overwatch")
		.add<Idle>()
		.add<Work>();

	sim.action("March")
		.add<Work>();

	sim.artefact("Patient 1")
		.add<Patient>()
		.entity()
		.add_switch<State>()
		.add_case<State::Unwell>()
		;

	//// -- System to print the beginning of a tick
	//std::cout << std::boolalpha; // Tells to ouput "true" or "false" instead of "1" or "0".
	//sim.world().system<>("TickBegin")
	//	.kind(flecs::PreFrame)
	//	.iter([](flecs::iter& iter)
	//		{
	//			std::cout << "-- Simulation : Tick " << iter.world().get_tick() + 1 << " - " << iter.delta_time() << "s\n";
	//		}
	//);

	//sim.world().system<const Patient>("PatientStatus")
	//	.term<State>().role(flecs::Switch)
	//	.kind(flecs::PreFrame)
	//	.iter([](flecs::iter& iter, const Patient * _)
	//		{
	//			std::cout << "- Patient status :" << std::endl;
	//			auto state = iter.term<flecs::entity_t>(2);
	//			bool all_safe{ false };
	//			for (auto i : iter)
	//			{
	//				std::cout << iter.entity(i).name() << " : " << iter.world().entity(state[i]).name() << "\n";
	//			}
	//			if (all_safe || iter.world().get_tick() >= 100)
	//			{
	//				iter.world().quit();
	//			}
	//		}
	//);

	//sim.world().system<const type::Agent>("AgentStatus")
	//	.kind(flecs::PreFrame)
	//	.iter([](flecs::iter& iter, const type::Agent * _)
	//		{
	//			std::cout << "- PVA status :" << std::endl;
	//			for (auto i : iter)
	//			{
	//				iter.entity(i).each<const act>([](flecs::entity action) { std::cout << action.name() << " "; });
	//			}
	//		}
	//);

 //   sim.world().system<const type::ProcessCounter>("ProcessCounter")
	//	.iter([](flecs::iter& it, const type::ProcessCounter * counter) {
	//			std::cout << "- Process status :" << std::endl;
	//			for (auto i : it)
	//			{
	//				std::cout << it.entity(i).name() << " : " << counter[i].value << "\n";
	//			}
 //       });

 //   sim.world().system<Result>("Logging")
	//	.each([](flecs::entity e, Result& result) {
	//			e.each<const act>([&result](flecs::entity action) { result.ss << action.name() << ";"; });
 //       });

	//sim.world().observer<const act>()
	//	.arg(1).obj(flecs::Wildcard)
	//	.event(flecs::OnAdd)
	//	.iter([](flecs::iter& iter, const act * _) {
	//			for (auto i : iter)
	//			{
	//				std::cout << iter.entity(i).name() << " is doing  ";
	//				iter.entity(i).each<const act>([](flecs::entity action) { std::cout << action.name() << "\n"; });
	//			}
	//		});


    sim.strategy<strat::ContainerAccumulator<std::vector<flecs::entity>>>()
        .behaviour(
            "Work",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent)
			{
				std::vector<flecs::entity> actions;
				agent.entity().world().each<const Work>([&agent, &actions](flecs::entity e, const Work _) {
					actions.push_back(e);
					});
				return actions;
			}
        )
        .behaviour(
            "Idle",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent) {
				std::vector<flecs::entity> actions;
				agent.entity().world().each<const Idle>([&agent, &actions](flecs::entity e, const Idle _) {
					actions.push_back(e);
					});
				return actions;
			}
        );

	sim.strategy<strat::InfluenceGraph<flecs::entity, std::vector<flecs::entity>>>()
		.behaviour(
			"First",
	        [](AgentHandle agent) -> bool {return true; },
	        [](AgentHandle agent, std::vector<flecs::entity> args)
			{
				std::vector<Influence<flecs::entity>> influences{};
				//for (const flecs::entity& e : args)
				//{
				//	influences.emplace_back(&e, true);
				//}
				return influences;
			}
	);

	sim.strategy<strat::Sequential<flecs::entity>>()
		.behaviour(
			"Execute",
	        [](AgentHandle agent) -> bool {return true; },
	        [](AgentHandle agent, flecs::entity arg)
			{
				agent.add<act>(arg);
				return arg;
			}
	);

	// -- Register some processes/reasonner
	// You must register them before populating the simulation.
	sim.agent_model<SimpleReasonner>();

	// -- Create some entities to populate the simulation;
	// First, let's create a prefab for our agents, or an archetype :
	auto archetype = sim.agent_archetype("Archetype_Basic")
		.add<type::Stress>()
		.add<Result>()
		.agent_model<SimpleReasonner>()
		;

	sim.agent(archetype, "PVA 1");
	sim.agent(archetype, "PVA 2");

	std::cout << "Launching...\n";
	std::cout << "------------\n";

	while(sim.step(1.0)){}

	std::cout << "-----------\n";
	std::cout << "Stopping...\n";

	sim.shutdown();

	sim.world().each([](flecs::entity e, const Result& result) {
		std::cout << e.name() << result.ss.str() << "\n";
		});
}