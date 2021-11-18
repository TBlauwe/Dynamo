#include <iostream>
#include <chrono>
#include <thread>

#include <ogdf/basic/Graph.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/fileformats/GraphIO.h>

#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>

struct Stress {
    float stress{ 100.0f };
};

struct Tag {};

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
                agent.set<Stress>({ 9.0f });
            }
        );
        t1.name("Random task");

        auto t2 = emplace([](AgentHandle agent)
            {
                agent.set<Stress>({ 12.0f });
            }
        );
        t2.name("Random task");

        auto process_a = process<strat::Random, std::string>();
        process_a.name("Random string");
        auto process_b = process<strat::Random, int>();
        process_b.name("Random int");
        auto process_c = process<strat::Random, std::string, std::string, int>(process_a, process_b);
        process_c.name("Random aggregator");
        //auto t4 = process<strat::InfluenceGraph<int>>(std::vector<int>{0,1,2,3,4,5});

        t1.succeed(t0);
    }
};

int main(int argc, char** argv) {


    // -----------------------------
    // SETUP
    // -----------------------------

    // -- Create an empty simulation
    Simulation sim;

    // -- System to print the beginning of a tick
    std::cout << std::boolalpha; // Tells to ouput "true" or "false" instead of "1" or "0".
    sim.world().system<>()
        .kind(flecs::PreFrame)
        .iter([](flecs::iter& iter)
            {
                std::cout << "\n -- Simulation : Tick " << iter.world().get_tick() << " - " << iter.delta_time() << "s" << std::endl;
            }
    );

    // -- Create some cognitive models

    // Stress passively decrease
    sim.world().system<Stress>()
        .kind(flecs::PreUpdate)
        .each([](flecs::entity entity, Stress& stress)
            {
                if (stress.stress > 0.f)
                {
                    stress.stress -= entity.delta_time();
                }
            }
    );

    // X. Experiment
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
                std::cout << "Oups " + arg + std::to_string(arg2) << "\n";
                return "Oups " + arg; 
            }
        )
        .behaviour(
            "MySecondBehaviour",
            [](AgentHandle agent) {return true; },
            [](AgentHandle agent, std::string arg, int arg2) {
                std::cout << "Arff " + arg + std::to_string(arg2) << "\n";
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
        .add<Stress>()
        .agent_model<SimpleReasonner>()
        ;

    // Then, we can create agent using our archetype :
    auto arthur = sim.agent(archetype, "Arthur");
    for (int i = 0; i < 0; i++) {
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


    sim.step_n(1000);

    // 5. Show graph
    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
        ogdf::GraphAttributes::nodeGraphics |
        ogdf::GraphAttributes::edgeGraphics |
        ogdf::GraphAttributes::nodeLabel |
        ogdf::GraphAttributes::edgeStyle |
        ogdf::GraphAttributes::nodeStyle |
        ogdf::GraphAttributes::nodeTemplate);

    std::cout << arthur.agent_models()[0];
    std::ifstream test(arthur.agent_models()[0]);
    ogdf::GraphIO::readDOT(GA, G, test);
    //ogdf::GraphIO::read(GA, G, std::istringstream(arthur.agent_models()[0]));


    // Layout
    for (ogdf::node v : G.nodes)
        GA.width(v) = GA.height(v) = 5.0;

    ogdf::SugiyamaLayout SL;
    SL.setRanking(new ogdf::OptimalRanking);
    SL.setCrossMin(new ogdf::MedianHeuristic);

    auto* ohl = new ogdf::OptimalHierarchyLayout;
    ohl->layerDistance(30.0);
    ohl->nodeDistance(25.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);

    SL.call(GA);
    ogdf::GraphIO::write(GA, "taskflow.svg", ogdf::GraphIO::drawSVG);

    sim.shutdown();

    return 0;
}