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
//#define FADIER


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

// Models; 
struct Demary {};
struct Driskell {};
struct Fadier {};

struct Mesure
{
	float ord{};
	float com{};
	float alu{};
	float coop{};
	float qual{};

	std::string to_csv() 
	{ 
		return 
			std::to_string(ord) + ";"
			+ std::to_string(com) + ";"
			+ std::to_string(alu) + ";"
			+ std::to_string(coop) + ";"
			+ std::to_string(qual) + ";"
			;
	}
};

std::ostream& operator<<(std::ostream& os, const Mesure& obj)
{
    // write obj to stream
	os << "ORD : " << obj.ord;
	os << " | COM : " << obj.com;
	os << " | ALU : " << obj.alu;
	os << " | COOP : " << obj.coop;
	os << " | QUAL : " << obj.qual;
    return os;
}

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

std::vector<Mesure> compute_mesure(const std::vector<flecs::entity>& vec)
{
	std::vector<Mesure> res{};
	for (auto agent : vec)
	{
		Mesure m
		{
			percentage(*agent.get<SelectionLog>(), M::ORD(agent)),
			percentage(*agent.get<SelectionLog>(), M::COM(agent)),
			percentage(*agent.get<SelectionLog>(), M::ALU(agent)),
			percentage(*agent.get<SelectionLog>(), M::COOP(agent)),
			percentage(*agent.get<SelectionLog>(), M::QUAL(agent))
		};
		res.push_back(m);
	}
	return res;
}

Mesure compute_mean(const std::vector<Mesure>& mesures)
{
	Mesure res{};
	for (const auto& m : mesures)
	{
		res.ord += m.ord;
		res.com += m.com;
		res.alu += m.alu;
		res.coop += m.coop;
		res.qual += m.qual;
	}
	res.ord /= mesures.size();
	res.com /= mesures.size() ;
	res.alu /=  mesures.size();
	res.coop /=  mesures.size();
	res.qual /=  mesures.size();

	return res;
}

Mesure compute_var(const std::vector<Mesure>& mesures, const Mesure& mean)
{
	Mesure res{};
	for (const auto& m : mesures)
	{
		res.ord += std::powf(m.ord - mean.ord, 2.0);
		res.com += std::powf(m.com - mean.com, 2.0);
		res.alu += std::powf(m.alu - mean.alu, 2.0);
		res.coop += std::powf(m.coop - mean.coop, 2.0);
		res.qual += std::powf(m.qual - mean.qual, 2.0);
	}
	res.ord /= mesures.size();
	res.com /= mesures.size() ;
	res.alu /=  mesures.size();
	res.coop /=  mesures.size();
	res.qual /=  mesures.size();

	return res;
}

void print_results(const std::vector<flecs::entity>& A, const std::vector<flecs::entity>& B)
{
	auto mesures_A = compute_mesure(A);
	auto mesures_B = compute_mesure(B);
	auto mean_A = compute_mean(mesures_A);
	auto mean_B = compute_mean(mesures_B);
	auto var_A = compute_var(mesures_A, mean_A);
	auto var_B = compute_var(mesures_B, mean_B);

	std::cout << "Group A avg : " << mean_A << "\n";
	std::cout << "Group B avg : " << mean_B << "\n";
	std::cout << "Group A var : " << var_A << "\n";
	std::cout << "Group B var : " << var_B << "\n";
}

void print_csv_mean(const char * name_a, const char * name_b, const std::vector<flecs::entity>& A, const std::vector<flecs::entity>& B)
{
	auto mesures_A = compute_mesure(A);
	auto mesures_B = compute_mesure(B);
	auto mean_A = compute_mean(mesures_A);
	auto mean_B = compute_mean(mesures_B);
	auto var_A = compute_var(mesures_A, mean_A);
	auto var_B = compute_var(mesures_B, mean_B);

	std::cout << name_a <<";" << mean_A.to_csv() << "\n";
	std::cout << name_b <<";" << mean_B.to_csv() << "\n";
}

void print_csv_var(const char * name_a, const char * name_b, const std::vector<flecs::entity>& A, const std::vector<flecs::entity>& B)
{
	auto mesures_A = compute_mesure(A);
	auto mesures_B = compute_mesure(B);
	auto mean_A = compute_mean(mesures_A);
	auto mean_B = compute_mean(mesures_B);
	auto var_A = compute_var(mesures_A, mean_A);
	auto var_B = compute_var(mesures_B, mean_B);

	std::cout << name_a <<";" << var_A.to_csv() << "\n";
	std::cout << name_b <<";" << var_B.to_csv() << "\n";
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
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
									influences.emplace_back(e, true);
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
	std::vector<int> trait_tag{ 1, 2, 3, 4 };
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

	std::vector<flecs::entity> group_demary{};
	std::vector<flecs::entity> group_driskell{};
	std::vector<flecs::entity> group_not_demary{};
	std::vector<flecs::entity> group_not_driskell{};
	std::vector<flecs::entity> group_fadier{};
	std::vector<flecs::entity> group_not_fadier{};

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

			if (!e.has<Proactive>())
				group_demary.push_back(e);
			else
				group_not_demary.push_back(e);

			if (e.has<Stressed>())
				group_driskell.push_back(e);
			else
				group_not_driskell.push_back(e);

			if (e.has<Experienced>())
				group_fadier.push_back(e);
			else
				group_not_fadier.push_back(e);
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

			p_follow_order += percentage(log, M::ORD(e));
			p_communicate += percentage(log, M::COM(e));
			p_follow_alu += percentage(log, M::ALU(e));
			p_cooperate += percentage(log, M::COOP(e));
			p_mastered += percentage(log, M::QUAL(e));

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

	std::cout << "----- COMPILATION -----\n";
#ifdef DEMARY
	std::cout << "MC Demary" << "\n";
#endif
#ifdef DRISKELL
	std::cout << "MC Driskell" << "\n";
#endif
#ifdef FADIER
	std::cout << "MC Fadier" << "\n";
#endif
	std::cout << "Group A : Demary" << "\n";
	std::cout << "Group B : Not demary" << "\n" ;
	print_results(group_demary, group_not_demary);

	std::cout << "-----------------------\n";
	std::cout << "Group A : Driskell" << "\n" ;
	std::cout << "Group B : Not driskell" << "\n" ;
	print_results(group_driskell, group_not_driskell);

	std::cout << "-----------------------\n";
	std::cout << "Group A : Fadier" << "\n" ;
	std::cout << "Group B : Not Fadier" << "\n" ;
	print_results(group_fadier, group_not_fadier);

	std::cout << "----- CSV -----\n";
	std::cout << "----- mean\n";
	print_csv_mean("Demary", "Not Demary", group_demary, group_not_demary);
	print_csv_mean("Driskell", "Not Driskell", group_driskell, group_not_driskell);
	print_csv_mean("Fadier", "Not Fadier", group_fadier, group_not_fadier);
	std::cout << "----- var\n";
	print_csv_var("Demary", "Not Demary", group_demary, group_not_demary);
	print_csv_var("Driskell", "Not Driskell", group_driskell, group_not_driskell);
	print_csv_var("Fadier", "Not Fadier", group_fadier, group_not_fadier);
}