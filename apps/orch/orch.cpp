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

struct Selection
{
	std::vector<flecs::entity> choices;
	flecs::entity choice;
};

struct SelectionLog
{
	std::vector<Selection> selection;
};


struct Counter
{
	int value{ 0 };
};

struct Message
{
	std::string	src	{ };
	std::string	room{ "CURRENT OPS" };
	std::string	dest{ };
	std::string	performatif{ "INFORM_REF" };
	std::string	content{ };
	bool show_dest{ true };
};

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

struct AAR_DO {};
struct CAS_DO {};
struct AWACS {};
struct Done {};
struct Unlimited {};

struct sending 
{
	Message value;
};

struct message_read 
{
	Message value;
};

struct informed 
{
};

struct confirmed 
{
};

struct Inform
{
	const char * agent;
};

std::ostream& operator<<(std::ostream& stream, const Message& m) {
	stream << m.src << " on [" << m.room << "]:" << (!m.dest.empty() && m.show_dest ? "to " + m.dest + " :" : "") << m.content;
    return stream;
 }


void add_action(flecs::entity agent, std::vector<flecs::entity>& actions, const char* name)
{
	auto action = agent.world().lookup(name);
	if(!agent.has<Done>(action) || action.has<Unlimited>())
		actions.push_back(action);
}

flecs::entity find_agent(flecs::entity agent, const char* name)
{
	return agent.world().lookup(name);
}

struct Test{};

int main(int argc, char** argv) {

	flecs::world w1{};
	w1.add<Test>();
	w1.add<Stressed>();
	flecs::world w2{};
	assert(w1.has<Test>() == true);
	assert(w2.has<Test>() == false);

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

	std::vector<flecs::entity> actions{
		sim.action("Do nothing")
		.add<Unlimited>()
		,
		sim.action("Inform AAR DO")
		.add<Communicative>()
		.set<Message>({"AWACS", "TAC C2 SUPPORT", "AAR DO", "INFORM_REF", "Tanker down"})
		,
		sim.action("ACK")
		.add<Communicative>()
		.add<Unlimited>()
		.set<Message>({"", "", "", "ACK", "c"})
		,
		sim.action("Inform Current OPS")
		.add<Communicative>()
		.set<Message>({"AAR DO", "CURRENT OPS", "CAS DO", "INFORM_REF", "Tanker down", false})
		,
		sim.action("Inform CAS DO")
		.add<Communicative>()
		.add<Proactive>()
		.set<Inform>({"CAS DO"})
		.set<Message>({"AAR DO", "TAC C2 OFFENSIVE", "CAS DO", "INFORM_REF", "Tanker down"})
	};

	sim.world().system<Counter>("Counter")
		.iter([&](flecs::iter& iter, Counter* counter)
			{
				for (auto i : iter)
				{
					if (counter[i].value > 10)
						iter.world().quit();
				}
			}
	);

	//sim.world().system<type::Agent>("Debug")
	//	.each([](flecs::entity entity, type::Agent _)
	//		{
	//			if (entity.has<CAS_DO>())
	//				std::cout << "Is informed : " << entity.has<informed>() << "\n";
	//		}
	//);

	sim.world().system<sending>("Message effect")
		.each([&](flecs::entity entity, sending& message)
			{
				flecs::entity dest = find_agent(entity, message.value.dest.c_str());
				if (dest.is_valid())
				{
					dest.set<message_read>({ message.value });
					if (message.value.performatif == "INFORM_REF")
					{
						if(!dest.has<Stressed>() || (dest.has<Stressed>() && message.value.dest == "CAS DO"))
							dest.add<informed>();
					}
				}
				entity.remove<sending>();
			}
	);

	sim.world().system<message_read, confirmed>("Message effect 2")
		.each([&](flecs::entity entity, message_read& message, confirmed _)
			{
				entity.remove<message_read>();
				entity.remove<confirmed>();
			}
	);

	sim.strategy<strat::ContainerAccumulator<std::vector<flecs::entity>>>()
		.behaviour(
			"Always",
			[](AgentHandle agent) {return true; },
			[](AgentHandle agent)
			{
				std::vector<flecs::entity> actions{};
				add_action(agent, actions, "Do nothing");
				if (agent.has<message_read>() && !agent.has<confirmed>() && agent.get<message_read>()->value.performatif != "ACK")
				{
					add_action(agent, actions, "ACK");
				}
				return actions;
			}
		)
		.behaviour(
			"AWACS",
			[](AgentHandle agent) {return agent.has<AWACS>(); },
			[](AgentHandle agent)
			{
				std::vector<flecs::entity> actions{};
				add_action(agent, actions, "Inform AAR DO");
				return actions;
			}
		)
		.behaviour(
			"AAR DO",
			[](AgentHandle agent) {return agent.has<AAR_DO>(); },
			[](AgentHandle agent)
			{
				std::vector<flecs::entity> actions{};
				if (agent.has<informed>())
				{
					add_action(agent, actions, "Inform Current OPS");
					if (!find_agent(agent, "CAS DO").has<informed>())
						add_action(agent, actions, "Inform CAS DO");
				}
				return actions;
			}
		);


	sim.strategy<strat::Sequential<flecs::entity>>()
		.behaviour(
			"Execute",
			[](AgentHandle agent) -> bool {return true; },
			[](AgentHandle agent, flecs::entity action)
			{
				agent.add<act>(action);
				agent.add<Done>(action);
				if (action.has<Message>())
				{
					Message m = Message(*action.get<Message>());
					m.src = agent.name();
					if (m.performatif == "ACK" && agent.has<message_read>())
					{
						auto ms = agent.get<message_read>()->value;
						if (rand() % 2 == 0)
							m.dest = ms.src;
						m.room = ms.room;
						agent.add<confirmed>();
					}
					std::cout << m << "\n";
					agent.set<sending>({m});
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
			.behaviour(
				"Proactive",
				[](AgentHandle agent) -> bool {return agent.has<Proactive>(); },
				[](AgentHandle agent, std::vector<flecs::entity> args)
				{
					std::vector<Influence<flecs::entity>> influences{};
					for (flecs::entity e : args)
					{
						if (e.has<Proactive>())
							influences.emplace_back(e, true);
					}
					return influences;
				}
			)
			.behaviour(
				"Communicant",
				[](AgentHandle agent) -> bool {return agent.has<Communicative>(); },
				[](AgentHandle agent, std::vector<flecs::entity> args)
				{
					std::vector<Influence<flecs::entity>> influences{};
					for (flecs::entity e : args)
					{
						if (e.has<Communicative>())
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
	auto archetype_sc1 = sim.agent_archetype("Archetype")
		.add<SelectionLog>()
		.agent_model<SimpleReasonner>()
		;

	sim.agent(archetype_sc1, "AAR DO")
		.add<AAR_DO>()
		;

	sim.agent(archetype_sc1, "CAS DO")
		.add<CAS_DO>()
		;

	sim.agent(archetype_sc1, "AWACS")
		.add<AWACS>()
		;

	while (sim.step(2.0f))

	sim.shutdown();
}