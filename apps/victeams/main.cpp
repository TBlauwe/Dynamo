#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <algorithm>

#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>
#include <dynamo/modules/basic_action.hpp>
#include <dynamo/modules/activity_dl.hpp>

using namespace dynamo;

struct act {};

// Tag; 
struct Idle {};
struct Work {};
struct Correct {};
struct Wrong {};
struct Leader {};
struct Ordered {};
struct Communicative {};
struct ALU {};

// Traits
struct ProActive{};

// Artefacts; 
struct Patient {};
struct State
{
	struct Well {};
	struct Unwell {};
};

// Misc
struct Active {};
struct Passive {};
struct NextStep {};
struct Unlimited {};

struct Order
{
	flecs::entity action;
};

struct Type
{
	struct Communication {};
	struct Physical {};
	struct Attention {};
};
struct Step
{
	size_t value{ 1 };
};

struct Selection
{
	std::vector<flecs::entity> choices;
	flecs::entity choice;
};

struct SelectionLog
{
	std::vector<Selection> selection;
};

struct Buddy
{
	flecs::entity value;
};

struct Done
{
	flecs::entity instigator;
};

float percentage(const SelectionLog& log, std::function<bool(const flecs::entity action)> match)
{
	float nb_proposed = 0;
	float nb_choosed = 0;
	for (const Selection& selection : log.selection)
	{
		auto count = std::count_if(selection.choices.begin(), selection.choices.end(), match);
		if (count)
		{
			nb_proposed++;
			if (match(selection.choice))
				nb_choosed++;
		}
	}
	return nb_choosed / nb_proposed;
}

void add_action(flecs::entity agent, std::vector<flecs::entity>& actions, const char* name)
{
	auto action = agent.world().lookup(name);
	if (!action.has<Done>())
		actions.push_back(action);
}

class SimpleReasonner : public AgentModel
{
public:
	using AgentModel::AgentModel;

	virtual constexpr const char* name() const { return "MySuperReasonner"; }

	void build() override
	{
		auto perception = emplace([](AgentHandle agent)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				agent.set<Step>({ agent.get<Step>()->value + 1 });
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

		auto logging = process_no<strat::Parallel, flecs::entity, std::vector<flecs::entity>>(selection, feasible);
	}
};

int main(int argc, char** argv) {

	// -----------------------------
	// SETUP
	// -----------------------------

	// -- Create an empty simulation
	Simulation sim{ 1 };
	sim.world().import<module::BasicStress>();
	sim.world().import<module::BasicAction>();
	sim.world().import<module::ADL>();

	sim.world().component<act>().add(flecs::Exclusive);

	sim.world().type<State>()
		.add<State::Well>()
		.add<State::Unwell>();


	// STEP 0 - Common
	sim.action("Do nothing")
		.add<Idle>()
		.add<Unlimited>()
		.set<type::Cost>({ 0 });

	sim.action("Say nothing")
		.add<Communicative>()
		.add<Unlimited>()
		.add<Idle>();

	sim.action("Overwatch")
		.add<Idle>()
		.add<Unlimited>()
		.add<Active>()
		.set<type::Cost>({ 0 });

	// STEP 1 - Analyse
	sim.action("Study")
		.add<Work>()
		.add<NextStep>()
		.set<type::Cost>({ 0 });

	// STEP 3 - Execute
	auto adre = sim.action("Inject adrenaline")
		.add<Work>()
		.add<Correct>()
		.add<NextStep>()
		.set<type::Cost>({ 0 });

	auto morph = sim.action("Inject morphine")
		.add<Work>()
		.add<Wrong>()
		.add<NextStep>()
		.set<type::Cost>({ 0 });

	// STEP 2 - Order
	sim.action("Order adrenaline")
		.add<Work>()
		.set<Order>({adre})
		.add<Communicative>()
		.set<type::Cost>({ 0 });

	sim.action("Order morphine")
		.add<Work>()
		.set<Order>({morph})
		.add<Communicative>()
		.set<type::Cost>({ 0 });


	// STEP 4 - Report
	sim.action("Report")
		.add<Communicative>()
		.add<Work>()
		.add<NextStep>()
		.set<type::Cost>({ 1 });

	// STEP 3 - Execute
	sim.artefact("Patient 1")
		.add<Patient>()
		.entity()
		.add_switch<State>()
		.add_case<State::Unwell>()
		;

	//// -- System to print the beginning of a tick
	std::cout << std::boolalpha; // Tells to ouput "true" or "false" instead of "1" or "0".
	sim.world().system<>("TickBegin")
		.kind(flecs::PreFrame)
		.iter([](flecs::iter& iter)
			{
				std::cout << "\n-- Simulation : Tick " << iter.world().get_tick() + 1 << " - " << iter.delta_time() << "s\n";
			}
	);

	sim.world().system<const Step>("EndCondition")
		.kind(flecs::PreFrame)
		.iter([](flecs::iter& iter, const Step* step)
			{
				for(auto i : iter)
				{
					if(step[i].value >= 5)
						iter.world().quit();
				}
			}
	);

	sim.world().system<const Patient>("PatientStatus")
		.term<State>().role(flecs::Switch)
		.kind(flecs::PreFrame)
		.iter([](flecs::iter& iter, const Patient* _)
			{
				auto state = iter.term<flecs::entity_t>(2);
				bool all_safe{ false };
				for (auto i : iter)
				{
					std::cout << iter.entity(i).name() << " : " << iter.world().entity(state[i]).name() << "\n";
				}
			}
	);

	sim.world().system<const type::Agent>("AgentStatus")
		.kind(flecs::PreFrame)
		.iter([](flecs::iter& iter, const type::Agent* _)
			{
				for (auto i : iter)
				{
					std::cout << iter.entity(i).name() << " is doing : ";
					iter.entity(i).each<const act>([](flecs::entity action) { std::cout << action.name() << ""; });
					std::cout << "\n";
				}
			}
	);

	// Configuration
	{
		sim.strategy<strat::ContainerAccumulator<std::vector<flecs::entity>>>()
			.behaviour(
				"Always",
				[](AgentHandle agent) {return true; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					add_action(agent.entity(), actions, "Do nothing");
					add_action(agent.entity(), actions, "Overwatch");
					return actions;
				}
			)
			.behaviour(
				"Order",
				[](AgentHandle agent) {return true; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					agent.entity().each<Ordered>([&](flecs::entity action) {
						if(!action.has<Done>())
							actions.push_back(action);
						});
					return actions;
				}
			)
			.behaviour(
				"Step I",
				[](AgentHandle agent) {return agent.get<Step>()->value == 1; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					add_action(agent.entity(), actions, "Study");
					return actions;
				}
			)
			.behaviour(
				"Step II",
				[](AgentHandle agent) {return agent.get<Step>()->value == 2; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					add_action(agent.entity(), actions, "Order adrenaline");
					add_action(agent.entity(), actions, "Order morphine");
					return actions;
				}
			)
			.behaviour(
				"Step III",
				[](AgentHandle agent) {return agent.get<Step>()->value == 3; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					add_action(agent.entity(), actions, "Inject adrenaline");
					add_action(agent.entity(), actions, "Inject morphine");
					return actions;
				}
			)
			.behaviour(
				"Step IV",
				[](AgentHandle agent) {return agent.get<Step>()->value == 4; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					add_action(agent.entity(), actions, "Report");
					return actions;
				}
			)
			.behaviour(
				"Idle",
				[](AgentHandle agent) {return true; },
				[](AgentHandle agent)
				{
					std::vector<flecs::entity> actions;
					agent.entity().world().each<const Idle>([&agent, &actions](flecs::entity e, const Idle _) {
						actions.push_back(e);
						});
					return actions;
				}
				);

				sim.strategy<strat::InfluenceGraph<flecs::entity, std::vector<flecs::entity>>>()
					.behaviour(
						"Passive",
						[](AgentHandle agent) -> bool {return !agent.has<ProActive>(); },
						[](AgentHandle agent, std::vector<flecs::entity> args)
						{
							std::vector<Influence<flecs::entity>> influences{};
							for (flecs::entity e : args)
							{
								if(e.has<Ordered>())
									influences.emplace_back(e, true);
								else if(e.has<ProActive>())
									influences.emplace_back(e, false);
							}
							return influences;
						})
					.behaviour(
						"ProActive",
						[](AgentHandle agent) -> bool {return agent.has<ProActive>(); },
						[](AgentHandle agent, std::vector<flecs::entity> args)
						{
							std::vector<Influence<flecs::entity>> influences{};
							for (flecs::entity e : args)
							{
								if(e.has<Idle>())
									influences.emplace_back(e, false);
								else if(e.has<ProActive>())
									influences.emplace_back(e, true);
							}
							return influences;
						}
				);

				sim.strategy<strat::Sequential<flecs::entity>>()
					.behaviour(
						"Execute",
						[](AgentHandle agent) -> bool {return true; },
						[](AgentHandle agent, flecs::entity action)
						{
							agent.add<act>(action);
							action.set<Done>({agent});

							if (action.has<NextStep>())
								agent.get_mut<Step>()->value++;

							if (action.has<Order>())
							{
								auto buddy = agent.get<Buddy>()->value;
								auto order = action.get<Order>()->action;
								buddy.add<Ordered>(order);
							}
							return action;
						}
				);

				sim.strategy<strat::Parallel<flecs::entity, std::vector<flecs::entity>>>()
					.behaviour(
						"Store",
						[](AgentHandle agent) -> bool {return true; },
						[](AgentHandle agent, flecs::entity choice, std::vector<flecs::entity> choices)
						{
							agent.get_mut<SelectionLog>()->selection.push_back({ choices, choice });
						}
				);
	}

	// -- Register some processes/reasonner
	// You must register them before populating the simulation.
	sim.agent_model<SimpleReasonner>();

	// -- Create some entities to populate the simulation;
	// First, let's create a prefab for our agents, or an archetype :
	auto archetype = sim.agent_archetype("Archetype_Basic")
		.add<Step>()
		.add<type::Stress>()
		.add<SelectionLog>()
		.agent_model<SimpleReasonner>()
		;

	// Configuration
	{
		auto pva_1 = sim.agent(archetype, "PVA 1");
		pva_1.add<Leader>();

		auto pva_2 = sim.agent(archetype, "PVA 2");
		pva_2.add<ProActive>();

		pva_2.set<Buddy>({ pva_1 });
		pva_1.set<Buddy>({ pva_2 });
	}


	std::cout << "Launching...\n";
	std::cout << "------------\n";

	while (sim.step(1.0)) {}

	std::cout << "-----------\n";
	std::cout << "Stopping...\n";

	sim.shutdown();


	float p_follow_order{ 0.0f };
	float p_communicate{ 0.0f };
	float p_follow_alu{ 0.0f };
	float p_cooperate{ 0.0f };
	float p_mastered{ 0.0f };
	int nb_agents = 0;

	sim.world().each([&](flecs::entity e, const SelectionLog& log)
		{
			nb_agents++;
			std::cout << "\n--" << e.name() << "--" << "\n";
			int i = 0;
			for (const Selection& choices : log.selection)
			{
				i++;
				std::cout << "[" << i << "] - " << "Choices :";
				for (const flecs::entity action : choices.choices)
				{
					std::cout << action.name() << " | ";
				}
				std::cout << "\nChoice : " << choices.choice.name() << "\n";
			}

			p_follow_order += percentage(log, [e](const flecs::entity action) {return action.has<Ordered>(); });
			p_communicate += percentage(log, [e](const flecs::entity action) {return action.has<Communicative>(); });
			p_follow_alu += percentage(log, [e](const flecs::entity action) {return action.has<ALU>(); });
			p_cooperate += percentage(log, [e](const flecs::entity action) {return e.has<Ordered>(action); });
			p_mastered += percentage(log, [e](const flecs::entity action) {return action.has<Ordered>(); });
		}
	);
	p_follow_order /= nb_agents;
	p_communicate /= nb_agents;
	p_follow_alu /= nb_agents;
	p_cooperate /= nb_agents;
	p_mastered /= nb_agents;

	std::cout << "----- RESULTS -----\n";
	std::cout << "Taux \"suivi ordre\" : " << p_follow_order << "\n";
	std::cout << "Taux \"communication\" : " << p_communicate << "\n";
	std::cout << "Taux \"ALU\" : " << p_follow_alu << "\n";
	std::cout << "Taux \"cooperation\" : " << p_cooperate << "\n";
	std::cout << "Taux \"mastered\" : " << p_mastered << "\n";
}