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

template<typename TOut>
class RandomStrategy : public Strategy<TOut>
{
public:

    TOut compute(Agent agent, std::vector<Behaviour<TOut>> active_behaviours) override
    {
        std::vector<Behaviour<TOut>> out{};
        std::sample(active_behaviours.begin(), active_behaviours.end(), std::back_inserter(out), 1, std::mt19937{ std::random_device{}() });
        return out[0](agent);
    }
};

struct TtTag {};

class TestReasonner : public Reasonner
{
public:
    TestReasonner(Agent agent) : Reasonner(agent) {}

private:
    void build() override
    {

        auto t0 = emplace([](Agent agent)
            {
                std::cout << agent.entity().name() << "is perceiving [";
                agent.entity().each<type::perceive>([](flecs::entity e)
                    {
                        std::cout << "-";
                    }
                );
                std::cout << "]" << std::endl;
                //agent.entity().add<TtTag>();
            }
        );

        auto t1 = emplace([](Agent agent)
            {
                std::cout << agent.entity() << " stress is " << agent.entity().get_mut<Stress>()->stress << std::endl;
            }
        );

        auto t2 = emplace([](Agent agent)
            {
                agent.entity().get_mut<Stress>()->stress = 1.0f;
                std::cout << agent.entity() << " my stress is now " << agent.entity().get_mut<Stress>()->stress << std::endl;
                //agent.entity().remove<type::IsProcessing>();
            });
        t1.succeed(t0);
        t2.succeed(t1);
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
            }
        );

        auto t1 = emplace([](Agent agent)
            {
                //std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << agent.entity() << " stress is " << agent.entity().get<Stress>()->stress << std::endl;
            }
        );

        auto t2 = emplace([](Agent agent)
            {
                //agent.entity().get_mut<Stress>()->stress = 1.0f;
                std::cout << agent.entity() << " my stress is now " << agent.entity().get<Stress>()->stress << std::endl;
                //agent.entity().remove<type::IsProcessing>();
            });
    }
};


int main(int argc, char** argv) {

    // -- Create an empty simulation
    Simulation sim;

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
        .perceived_by(bob)
        .perceived_by(arthur)
        ;

    // X. Experiment
    RandomStrategy<std::string> random_strat;
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
    //auto out = random_strat(arthur);

    // 5. Show graph
    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
        ogdf::GraphAttributes::nodeGraphics |
        ogdf::GraphAttributes::edgeGraphics |
        ogdf::GraphAttributes::nodeLabel |
        ogdf::GraphAttributes::edgeStyle |
        ogdf::GraphAttributes::nodeStyle |
        ogdf::GraphAttributes::nodeTemplate);
/
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

    sim.step_n(500, 1.5f);
    sim.executor.wait_for_all();

    return 0;
}