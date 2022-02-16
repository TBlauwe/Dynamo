#include <iostream>
#include <chrono>
#include <thread>

#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>


using namespace dynamo;

struct Test1 {};
struct Test2 {};
struct Test3 {};
struct Test4 {};
struct Test5 {};
struct Test6 {};
struct Test7 {};
struct Test8 {};
struct Test9 {};
struct Test10 {};
struct Test11 {};

class SimpleReasonner : public FlowBuilder
{
public:
    using FlowBuilder::FlowBuilder;

    virtual constexpr const char* name() const { return "MySuperReasonner"; }

    void build() override
    {
        auto t0 = emplace([](AgentHandle agent)
            {
                agent.has<Stress>();
                agent.has<Test1>();
                agent.has<Test2>();
                agent.has<Test3>();
                agent.has<Test5>();
                agent.has<Test7>();
                agent.has<Test9>();
                agent.has<Test11>();
            }
        );
        t0.name("Random task");

        auto t1 = emplace([](AgentHandle agent)
            {
                agent.set<Stress>({ 9.0f });
            }
        );
        t1.name("Random task");

        //auto t2 = emplace([](AgentHandle agent)
        //    {
        //        //auto f = agent.entity().world().query<const type::Agent>();
        //        //f.each([](flecs::entity e, const type::Agent) {});
        //    }
        //);
        //t2.name("Random task");

        //t0.succeed(t1, t2);

        //auto process_a = process<strat::Random, std::string>();
        //process_a.succeed(t0);
        //auto process_b = process<strat::Random, int>();
        //process_b.name("Random int");
        //auto process_c = process<strat::Random, std::string, std::string, int>(process_a, process_b);
        //process_c.name("Random aggregator");
        //process_c.input_name(process_a, "From random string");
        //process_c.input_name(process_b, "From random int");
    }
};

int main(int argc, char** argv) {

    const size_t number_of_agents   = 1400;
    const size_t number_of_ticks    = 50000;
    const size_t number_of_threads  = std::thread::hardware_concurrency();

    std::cout << " -- PARAMETERS --" << std::endl;
    std::cout << "Number of agents  : " << number_of_agents << std::endl;
    std::cout << "Number of ticks   : " << number_of_ticks << std::endl;
    std::cout << "Number of threads : " << number_of_threads << std::endl;
    std::cout << std::endl;
    std::cout << " -- START --" << std::endl;
    const auto start_time = std::chrono::system_clock::now();
    std::cout << "setup in progress ...";

    // -----------------------------
    // SETUP
    // -----------------------------

    // -- Create an empty simulation
    Simulation sim{number_of_threads};
    sim.world().import<module::BasicStress>();

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

    // -- Register some processes/reasonner
    // You must register them before populating the simulation.
    sim.flow<SimpleReasonner>();


    // -- Create some entities to populate the simulation;
    // First, let's create a prefab for our agents, or an archetype :

    auto archetype = sim.agent_archetype("Archetype_Basic")
        .add<Test1>()
        .add<Test3>()
        .add<Test5>()
        .add<Test7>()
        .add<Test9>()
        .add<Test11>()
        .add<Stress>();

	archetype.flow<SimpleReasonner>({true, 1.0f});

    // Then, we can create agent using our archetype :
    for (int i = 0; i < number_of_agents; i++) {
        sim.agent(archetype, fmt::format("Agent {}", i).c_str());
    }

    std::cout << "Done !\n"; 

    int num_flow{ 0 };
    sim.world().each<const Flow>([&num_flow](flecs::entity e, const Flow& flow) { num_flow++; });
    std::cout << "Num of flow : " << num_flow << "\n";

    std::cout << "Launching ..." << std::endl;
    sim.step_n(number_of_ticks);

    std::cout << "Waiting for reasonning to finish ..." << std::endl;
    const std::chrono::duration<double, std::milli> duration_ticks = std::chrono::system_clock::now() - start_time;
    sim.shutdown();

    const std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start_time;
    std::cout << "Done waiting ! " << std::endl;
    std::cout << "Total elapsed time : " << duration.count() << "ms" << std::endl;

    std::cout << " -- END --\n\n";

    std::cout << " -- STATISTICS --" << std::endl;

    std::chrono::duration<double, std::milli> ben_tps{ 884.9 };

    std::cout << "             Total time elapsed : " << duration_ticks.count() << "ms" << std::endl;
    std::cout << "          Average time per tick : " << duration_ticks.count() / number_of_ticks << "ms" << std::endl;
    std::cout << "    Average time per tick (BEN) : " << ben_tps.count() << "ms" << std::endl;
    std::cout << "                          Ratio : " << ben_tps.count() / (duration_ticks.count() / number_of_ticks) << "x" << std::endl;
    std::cout << "Average time per tick per agent : " << duration_ticks.count() / number_of_ticks / number_of_agents << "ms" << std::endl;
    std::cout << "---" << std::endl;

    int sum{ 0 };
    sim.world().each([&sum](flecs::entity e, const Counter counter) {
        sum += counter.value;
        });

    double avg{ 0 };
    sim.world().each([&avg](flecs::entity e, const Duration duration) {
        avg += (duration.value.count() / e.get<Counter>()->value);
        });

    std::cout << "                      Number of process computed : " << sum << std::endl;
    std::cout << "                  Time spent computing processes : " << avg << "ms" << std::endl;
    avg /= number_of_agents;
    std::cout << "Average time spent computing processes per agent : " << avg << "ms" << std::endl;
    std::cout << "     Average count of process computed per agent : " << sum / number_of_agents << std::endl;
    std::cout << "               Average process computed per tick : " << sum / number_of_ticks << std::endl;
    std::cout << "           Average process computed per tick BEN : " << number_of_agents << std::endl;
    std::cout << "                                            Diff : " << number_of_agents / (sum / (number_of_ticks * 1.0f)) << "x" << std::endl;

    return 0;
}