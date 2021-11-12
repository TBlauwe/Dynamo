#include <iostream>
#include <chrono>
#include <thread>

#include <ogdf/basic/Graph.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/fileformats/GraphIO.h>

#include <dynamo/simulation.hpp>

struct Stress {
    float stress{ 100.0f };
};

struct Tag {};

using namespace dynamo;

template<typename TOutput, typename ... TInputs>
class RandomStrategy : public Strategy<TOutput, TOutput, TInputs ...>
{
    using Behaviour_t = Behaviour<TOutput, TInputs...>;
public:

    TOutput compute(Agent agent, std::vector<Behaviour_t> active_behaviours, TInputs ... inputs) const override
    {
        std::vector<Behaviour_t> out{};
        std::sample(active_behaviours.begin(), active_behaviours.end(), std::back_inserter(out), 1, std::mt19937{ std::random_device{}() });
        return out[0](agent, inputs ...);
    }
};

class SimpleReasonner : public Reasonner
{
public:
    SimpleReasonner(Agent agent) : Reasonner(agent) {}

private:
    void build() override
    {

        auto t0 = emplace([](Agent agent)
            {
                std::cout << "Stress : " << agent.entity().get<Stress>()->stress << "\n";;
            }
        );

        auto t1 = emplace([](Agent agent)
            {
            }
        );

        auto t2 = emplace([](Agent agent)
            {
            }
        );

        t1.succeed(t0);
    }
};

template<typename T>
void test()
{
    std::cout << typeid(T).name() << std::endl;
}

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

    // -- Register some processes/reasonner
    // You must register them before populating the simulation.
    sim.register_reasonner<SimpleReasonner>();


    // -- Create some entities to populate the simulation;
    // First, let's create a prefab for our agents, or an archetype :
    auto archetype = sim.agent_archetype("Archetype_Basic")
        .add<Stress>()
        .add_reasonner<SimpleReasonner>()
        ;

    auto another_archetype = sim.agent_archetype(archetype, "Archetype_Advanced")
        .add_shared<Tag>();

    // Then, we can create agent using our archetype :
    auto arthur = sim.agent(archetype, "Arthur");
    sim.agent(archetype, "Arthur1");
    sim.agent(archetype, "Arthur2");
    sim.agent(archetype, "Arthur3");
    sim.agent(archetype, "Arthur4");
    sim.agent(archetype, "Arthur5");
    sim.agent(archetype, "Arthur6");
    sim.agent(archetype, "Arthur7");
    sim.agent(archetype, "Arthur8");
    auto bob = sim.agent(another_archetype, "Bob");

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
        //.perceived_by(bob)
        .perceived_by(arthur)
        ;

    // X. Experiment
    //auto* random_strat = sim.add<RandomStrategy<std::string>>(); 
    auto& random_strat = sim.add<RandomStrategy<std::string>>(); 
    random_strat.add(Behaviour<std::string>{
        "MyFirstBehaviour",
            [](Agent agent) -> bool {return true; },
            [](Agent agent) -> std::string {return "Yeah"; }
    });
    random_strat.add(Behaviour<std::string>{
        "MySecondBehaviour",
            [](Agent agent) -> bool {return true; },
            [](Agent agent) -> std::string {return "Nay (but Yeah!)"; }
    });

    tf::Taskflow taskflow;
    taskflow.emplace([&sim, arthur]() {sim.get<RandomStrategy<std::string>>()(arthur); });
    sim.executor.run(taskflow);

    // 5. Show graph
    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
        ogdf::GraphAttributes::nodeGraphics |
        ogdf::GraphAttributes::edgeGraphics |
        ogdf::GraphAttributes::nodeLabel |
        ogdf::GraphAttributes::edgeStyle |
        ogdf::GraphAttributes::nodeStyle |
        ogdf::GraphAttributes::nodeTemplate);

    ogdf::node node_a = G.newNode();
    ogdf::node node_b = G.newNode();
    ogdf::node node_c = G.newNode();
    GA.label(node_a) = "A";
    GA.label(node_b) = "B";
    GA.label(node_c) = "C";
    G.newEdge(node_a, node_b);
    G.newEdge(node_a, node_c);


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

    sim.step_n(100);
    //tf::Taskflow main;
    //main.emplace([&tick]() {
    //    tick.run(); 
    //    });
    //for (int i = 0; i < 100; i++) {
    //    sim.world().frame_begin();
    //    sim.executor.run(main).wait();
    //    std::cout << sim.world().delta_time();
    //    sim.world().frame_end();
    //}
    sim.shutdown();

    return 0;
}