#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <array>
#include <algorithm>

#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>
#include <dynamo/modules/basic_action.hpp>
#include <dynamo/modules/activity_dl.hpp>

#define DEMARY
#define DRISKELL
#define FADIER

template <typename T>
std::vector<std::vector<T>> combinatorial(const std::vector<T> container, int n)
{
	std::vector<std::vector<T>> result{};
	int cap = 1 << n;
	for (int i = 1; i < cap; ++i)
	{
		std::vector<T> comb{};
		for (int j = 0; j < n; ++j)
		{
			if (i & (1 << j))
				comb.push_back(container.at(j));
		}
		result.push_back(comb);
	}
	return result;
}

template <typename T>
bool contains(const std::vector<T>& vec, T&& elem)
{
	if (std::find(vec.begin(), vec.end(), elem) != vec.end())
	{
		return true;
	}
	return false;
}

using namespace dynamo;

struct act {};
enum class Qualif
{
	None,
	SC1,
	SC2,
	SC3,
};

// Tag; 
struct Order {};
struct ALU {};
struct Communication {};
struct Coop {};

// Traits; 
struct Communicative {};
struct Proactive {};
struct Stressed {};
struct Experienced {};

struct Qualification
{
	Qualif value{ Qualif::None };
};

namespace M
{
	struct Schema
	{
		Schema(flecs::entity agent) : agent{ agent } {};
		virtual bool operator()(flecs::entity action) = 0;
	protected:
		flecs::entity agent;
	};

	struct ORD : Schema
	{
		using Schema::Schema;
		bool operator()(const flecs::entity action) override
		{
			return action.has<Order>();
		}
	};

	struct COM : Schema
	{
		using Schema::Schema;
		bool operator()(const flecs::entity action) override
		{
			return action.has<Communication>();
		}
	};

	struct ALU : Schema
	{
		using Schema::Schema;
		bool operator()(const flecs::entity action) override
		{
			return action.has<::ALU>();
		}
	};

	struct COOP : Schema
	{
		using Schema::Schema;
		bool operator()(const flecs::entity action) override
		{
			return action.has<Coop>();
		}
	};

	struct QUAL : Schema
	{
		using Schema::Schema;
		bool operator()(const flecs::entity action) override
		{
			return agent.get<Qualification>()->value >= action.get<Qualification>()->value;
		}
	};
}

struct Counter
{
	size_t value{ 0 };
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

float taux(const std::vector<flecs::entity>& vec, std::function<bool(const flecs::entity action)> match)
{
	float count = std::count_if(vec.begin(), vec.end(), match);
	return (count / static_cast<float>(vec.size())) * 100;
}

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
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				agent.get_mut<Counter>()->value++;
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

	sim.world().system<>("Tick")
		.iter([](flecs::iter& iter)
			{
				std::cout << "- Tick [" << iter.world().tick() << "]\n";
			});

	//struct Order {}; 2
	//struct ALU {}; 2
	//struct Communication {}; 2
	//struct Coop {}; 2
	//struct Qualif {}; 3
	std::vector<int> action_tag{ 1, 2, 3, 4 };
	auto action_tag_comb = combinatorial(action_tag, action_tag.size());

	std::vector<flecs::entity> actions{
		sim.action("Do nothing")
		.set<Qualification>({Qualif::None})
		,
		sim.action("Ne pas mettre des gants")
		.add<ALU>()
		.set<Qualification>({Qualif::SC3})
		,
		sim.action("Analyse")
		.add<Coop>()
		.set<Qualification>({Qualif::SC2})
		,
		sim.action("Inject adrenaline")
		.add<Coop>()
		.set<Qualification>({Qualif::SC2})
		,
		sim.action("Inject morphine")
		.add<Coop>()
		.add<Order>()
		.set<Qualification>({Qualif::SC2})
		,
		sim.action("Order adrenaline")
		.add<Communication>()
		.set<Qualification>({Qualif::SC1})
		,
		sim.action("Order morphine")
		.add<Communication>()
		.set<Qualification>({Qualif::SC1})
		,
		sim.action("Report")
		.add<Communication>()
		.add<Coop>()
		.set<Qualification>({Qualif::SC1})
	};
	auto comb = combinatorial(actions, actions.size());
	std::sort(comb.begin(), comb.end(), [](const auto& a, const auto& b) { return a.size() < b.size(); });

	sim.world().system<Counter>("Counter")
		.iter([&](flecs::iter& iter, Counter* counter)
			{
				for (auto i : iter)
				{
					if (counter[i].value >= comb.size())
						iter.world().quit();
				}
			});

	sim.strategy<strat::ContainerAccumulator<std::vector<flecs::entity>>>()
		.behaviour(
			"Always",
			[](AgentHandle agent) {return true; },
			[comb](AgentHandle agent)
			{
				auto index = agent.get<Counter>()->value;
				return comb[index - 1];
			}
	);

	sim.strategy<strat::Sequential<flecs::entity>>()
		.behaviour(
			"Execute",
			[](AgentHandle agent) -> bool {return true; },
			[](AgentHandle agent, flecs::entity action)
			{
				agent.add<act>(action);
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

	// Configuration
	{
		sim.strategy<strat::InfluenceGraph<flecs::entity, std::vector<flecs::entity>>>()
#ifdef DEMARY
			.behaviour(
				"Passive",
				[](AgentHandle agent) -> bool {return !agent.has<Proactive>(); },
				[](AgentHandle agent, std::vector<flecs::entity> args)
				{
					std::vector<Influence<flecs::entity>> influences{};
					for (flecs::entity e : args)
					{
						if (e.has<Order>())
							influences.emplace_back(e, true);
					}
					return influences;
				}
			)
#endif
#ifdef DRISKELL
			.behaviour(
				"Driskell-1",
				[](AgentHandle agent) -> bool {return agent.has<Stressed>(); },
				[](AgentHandle agent, std::vector<flecs::entity> args)
				{
					std::vector<Influence<flecs::entity>> influences{};
					for (flecs::entity e : args)
					{
						if (e.has<Coop>() || e.has<Communicative>())
							influences.emplace_back(e, false);
					}
					return influences;
				}
			)
					.behaviour(
						"Driskell-2",
						[](AgentHandle agent) -> bool {return agent.has<Stressed>(); },
						[](AgentHandle agent, std::vector<flecs::entity> args)
						{
							std::vector<Influence<flecs::entity>> influences{};
							for (flecs::entity e : args)
							{
								if (agent.get<Qualification>()->value >= e.get<Qualification>()->value)
									influences.emplace_back(e, false);
							}
							return influences;
						}
					)
#endif
#ifdef FADIER
					.behaviour(
						"Fadier",
						[](AgentHandle agent) -> bool {return agent.has<Experienced>(); },
						[](AgentHandle agent, std::vector<flecs::entity> args)
						{
							std::vector<Influence<flecs::entity>> influences{};
							for (flecs::entity e : args)
							{
								if (e.has<ALU>())
									influences.emplace_back(e, true);
							}
							return influences;
						}
					)
#endif
							;
	}
	// -- Register some processes/reasonner
	// You must register them before populating the simulation.
	sim.agent_model<SimpleReasonner>();

	// -- Create some entities to populate the simulation;
	// First, let's create a prefab for our agents, or an archetype :
	auto archetype_sc1 = sim.agent_archetype("Archetype_SC1")
		.add<SelectionLog>()
		.add<Counter>()
		.set<Qualification>({ Qualif::SC1 })
		.agent_model<SimpleReasonner>()
		;

	auto archetype_sc2 = sim.agent_archetype("Archetype_SC2")
		.add<SelectionLog>()
		.add<Counter>()
		.set<Qualification>({ Qualif::SC2 })
		.agent_model<SimpleReasonner>()
		;

	auto archetype_sc3 = sim.agent_archetype("Archetype_SC3")
		.add<SelectionLog>()
		.add<Counter>()
		.set<Qualification>({ Qualif::SC3 })
		.agent_model<SimpleReasonner>()
		;

	//auto groupe_driskell = sim.world().query_builder<const SelectionLog>()

	//struct Communicative {}; 2
	//struct Proactive {}; 2
	//struct Stressed {}; 2
	//struct Experienced {}; 2
	//struct Qualif {}; 3
	std::vector<int> trait_tag{ 1, 2, 3, 4};
	auto trait_tag_comb = combinatorial(trait_tag, trait_tag.size());

	flecs::entity agent;
	for (const std::vector<int>& trait_comb : trait_tag_comb)
	{
		auto sc1 = sim.agent(archetype_sc1);
		auto sc2 = sim.agent(archetype_sc2);
		auto sc3 = sim.agent(archetype_sc3);
		if (contains(trait_comb, 1))
		{
			sc1.add<Communicative>();
			sc2.add<Communicative>();
			sc3.add<Communicative>();
		}
		if (contains(trait_comb, 2))
		{
			sc1.add<Proactive>();
			sc2.add<Proactive>();
			sc3.add<Proactive>();
		}
		if (contains(trait_comb, 3))
		{
			sc1.add<Stressed>();
			sc2.add<Stressed>();
			sc3.add<Stressed>();
		}
		if (contains(trait_comb, 4))
		{
			sc1.add<Experienced>();
			sc2.add<Experienced>();
			sc3.add<Experienced>();
		}

		agent = sc2;
	}


	std::cout << "--- Actions combinaisons \n";
	std::cout << "List : " << "\n";
	for (const auto& c : comb)
	{
		std::cout << "[";
		for (const auto& e : c)
		{
			std::cout << e.name() << " ";
		}
		std::cout << "]";
		std::cout << " | ORD " << taux(c, M::ORD(agent)) << "%";
		std::cout << " | COM " << taux(c, M::COM(agent)) << "%";
		std::cout << " | COOP " << taux(c, M::COOP(agent)) << "%";
		std::cout << " | ALU " << taux(c, M::ALU(agent)) << "%";
		std::cout << " | QUAL " << taux(c, M::QUAL(agent)) << "%";
		std::cout << "\n";
	}
	std::cout << "Total : " << comb.size() << "\n";


	std::cout << "Launching...\n";
	std::cout << "------------\n";

	while (sim.step(2.0f))

		std::cout << "-----------\n";
	std::cout << "Stopping...\n";

	sim.shutdown();


	std::cout << "----- RESULTS -----\n";
	int nb_agent{ 0 };
	sim.world().each([&nb_agent](flecs::entity e, const type::Agent _) { nb_agent++; });
	std::cout << "Nombre d'agents : " << nb_agent << "\n";

	sim.world().each([&](flecs::entity e, const SelectionLog& log)
		{
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
		}
	);

	sim.world().each([&](flecs::entity e, const SelectionLog& log)
		{
			float p_follow_order{ 0.0f };
			float p_communicate{ 0.0f };
			float p_follow_alu{ 0.0f };
			float p_cooperate{ 0.0f };
			float p_mastered{ 0.0f };
			std::cout << "\n--" << e.name() << "--" << "\n";

			p_follow_order += percentage(log, [e](const flecs::entity action) {return action.has<Order>(); });
			p_communicate += percentage(log, [e](const flecs::entity action) {return action.has<Communication>(); });
			p_follow_alu += percentage(log, [e](const flecs::entity action) {return action.has<ALU>(); });
			p_cooperate += percentage(log, [e](const flecs::entity action) {return action.has<Coop>(); });
			p_mastered += percentage(log, [e](const flecs::entity action) {return e.get<Qualification>()->value >= action.get<Qualification>()->value; });

			std::cout << "--- Agent : " << e.name() << "\n";
			std::cout << "Profil : ";
			if (e.has<Communicative>()) std::cout << "Communicative ";
			if (e.has<Proactive>()) std::cout << "Proactive ";
			if (e.has<Stressed>()) std::cout << "Stressed ";
			std::cout << " SC_" << static_cast<int>(e.get<Qualification>()->value) << "\n";

			std::cout << "Taux \"suivi ordre\" : " << p_follow_order << "\n";
			std::cout << "Taux \"communication\" : " << p_communicate << "\n";
			std::cout << "Taux \"ALU\" : " << p_follow_alu << "\n";
			std::cout << "Taux \"cooperation\" : " << p_cooperate << "\n";
			std::cout << "Taux \"mastered\" : " << p_mastered << "\n";
		}
	);
}