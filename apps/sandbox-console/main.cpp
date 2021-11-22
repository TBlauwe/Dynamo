#include <iostream>
#include <chrono>
#include <thread>

#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>

using namespace dynamo;

class SimpleReasonner : public AgentModel
{
public:
    SimpleReasonner(Strategies const * const strategies, AgentHandle agent) : AgentModel(strategies, agent) {}

private:

    virtual constexpr const char* name() const { return "MySuperReasonner"; }

    void build() override
    {
        auto t0 = emplace([](AgentHandle agent)
            {
            }
        );
        t0.name("Random task");

        auto t1 = emplace([](AgentHandle agent)
            {
                agent.set<type::Stress>({ 9.0f });
            }
        );
        t1.name("Random task");

        auto t2 = emplace([](AgentHandle agent)
            {
                agent.set<type::Stress>({ 12.0f });
            }
        );
        t2.name("Random task");

        t0.succeed(t1, t2);

        auto process_a = process<strat::Random, std::string>();
        process_a.name("Random string");
        process_a.succeed(t0);
        auto process_b = process<strat::Random, int>();
        process_b.name("Random int");
        auto process_c = process<strat::Random, std::string, std::string, int>(process_a, process_b);
        process_c.name("Random aggregator");

        //auto v = static_value(std::vector<int>{0, 1, 2, 3, 4, 5});
        //auto t4 = process<strat::InfluenceGraph, int, std::vector<int>>(v);
    }
};

int main(int argc, char** argv) {

    const size_t number_of_agents = 100;
    const size_t number_of_ticks = 10000;
    const size_t number_of_threads = std::thread::hardware_concurrency();

    std::cout << "Number of agents : " << number_of_agents << std::endl;
    std::cout << "Number of ticks : " << number_of_ticks << std::endl;
    std::cout << "Number of threads : " << number_of_threads << std::endl;
    std::cout << " -- Simulation starting --" << std::endl;
    const auto start_time = std::chrono::system_clock::now();

    // -----------------------------
    // SETUP
    // -----------------------------

    // -- Create an empty simulation
    Simulation sim{number_of_threads};
    sim.world().import<module::BasicStress>();

    // -- System to print the beginning of a tick
    //std::cout << std::boolalpha; // Tells to ouput "true" or "false" instead of "1" or "0".
    //sim.world().system<>()
    //    .kind(flecs::PreFrame)
    //    .iter([](flecs::iter& iter)
    //        {
    //            std::cout << "\n -- Simulation : Tick " << iter.world().get_tick() << " - " << iter.delta_time() << "s" << std::endl;
    //        }
    //);

    // -- Define strategies and behaviours
    sim.strategy<strat::Random<std::string>>()
        .behaviour(
            "MyFirstBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent) {return "Yeah"; }
        )
        .behaviour(
            "MySecondBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent) {return "Nay (but Yeah!)"; }
        );

    sim.strategy<strat::Random<int>>()
        .behaviour(
            "MyFirstBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent) {return 0; }
        )
        .behaviour(
            "MySecondBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent) {return 1; }
        );

    sim.strategy<strat::Random<std::string, std::string, int>>()
        .behaviour(
            "MyFirstBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent, std::string arg, int arg2) {
                return "Oups " + arg; 
            }
        )
        .behaviour(
            "MySecondBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent, std::string arg, int arg2) {
                return "Arff " + arg; }
        );

    //auto& influence_graph_strat = sim.add<strat::InfluenceGraph<int>>();
    //influence_graph_strat.add(Behaviour<std::vector<dynamo::strat::Influence<int>>, const std::vector<int const*>&>{
    //    "MyOtherBehaviour",
    //        [](AgentHandle agent) -> bool {return true; },
    //        [](AgentHandle agent) -> std::string {return "Yeah"; }
    //});

    // -- Register some processes/reasonner
    // You must register them before populating the simulation.
    sim.agent_model<SimpleReasonner>();


    // -- Create some entities to populate the simulation;
    // First, let's create a prefab for our agents, or an archetype :
    auto archetype = sim.agent_archetype("Archetype_Basic")
        .add<type::Stress>()
        .agent_model<SimpleReasonner>()
        ;

    // Then, we can create agent using our archetype :
    auto arthur = sim.agent(archetype, "Arthur");
    for (int i = 0; i < number_of_agents; i++) {
        sim.agent(archetype, fmt::format("Agent {}", i).c_str());
    }

    std::vector<flecs::entity_view> agents{};
    sim.for_each([&agents](flecs::entity agent, type::Agent& _) {
        agents.emplace_back(agent);
        });

    auto radio = sim.artefact("Radio");
    radio.entity()
        .set<dynamo::type::PeriodicEmitter, dynamo::type::Message>({ 2.5f })
        .set<dynamo::type::Targets>({ agents });

    // 3. Create a percept seen by all entities
    sim.percept<type::Hearing>(radio)
        .perceived_by(radio)
        .perceived_by(arthur)
        ;

    sim.step_n(number_of_ticks);

    const std::chrono::duration<double, std::milli> duration_ticks = std::chrono::system_clock::now() - start_time;
    std::cout << " -- Simulation (ticks) ending in : " << duration_ticks.count() << "ms" << std::endl;
    std::cout << "Waiting for reasonning to finish ..." << std::endl;
    sim.shutdown();

    const std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start_time;
    std::cout << "Done waiting ! " << std::endl;
    std::cout << "Total elapsed time : " << duration.count() << "ms" << std::endl;

    return 0;
}