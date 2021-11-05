#include <dynamo/simulation.hpp>
#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/fileformats/GraphIO.h>

struct Stress {
	float stress{ 100.0f };
};

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

class TestReasonner : public Reasonner
{
public:
	TestReasonner(Agent agent) : Reasonner(agent) {}

private:
	void build() override
	{
		auto t1 = emplace([](Agent agent) {
			auto* stress = agent.entity().get_mut<Stress>();
			std::cout << "Hey ! I'm " << agent.entity() << " and my stress is " << stress->stress << "\n";
			});

		auto t2 = emplace([](Agent agent) {
			auto* stress = agent.entity().get_mut<Stress>();
			stress->stress = 1.0f;
			std::cout << "Hey again ! I'm " << agent.entity() << " and my stress is " << stress->stress << "\n";
			});
		t2.succeed(t1);

		emplace([](Agent agent) {
			const auto* stress = agent.entity().get<Stress>();
			std::cout << "Hey ! I'm " << agent.entity() << " and my stress is " << stress->stress << "\n";
			});
	}
};


int main(int argc, char** argv) {

	// Create an empty simulation
	Simulation sim;

	// 1. Define an observer to print who is seeing what
	sim.world().observer<>()
		.term<::dynamo::type::Agent>()
		.term<relation::perceive>(flecs::Wildcard)
		.event(flecs::OnAdd)
		.iter([](flecs::iter& iter) {
		std::cout << "Triggered \n";
		auto object = iter.term_id(2).object();
		for (auto i : iter) {
			std::cout << "Entity      : " << iter.entity(i).name() << "\n";
			std::cout << "Object      : " << object.name() << "\n";
			std::cout << "Object Type : " << object.type().str() << "\n";
		}
			}
	);

	// Create some cognitive models
	sim.world().system<Stress>()
		.kind(flecs::PreUpdate)
		.each([](flecs::entity entity, Stress& stress) {
		if (stress.stress > 0.f) {
			stress.stress -= entity.delta_time();
		}
			});

	// 2. Create some entities to populate simulation;
	auto arthur = sim.agent("Arthur");
	arthur.entity().add<Stress>();
	auto bob = sim.agent("Bob");
	auto radio = sim.artefact("Radio");

	// 3. Create a percept seen by all entities
	sim.percept<senses::Hearing>(radio)
		.perceived_by(radio)
		.perceived_by(bob)
		.perceived_by(arthur)
		;


	// 4. Register reasonner so that they can be triggered
	sim.register_reasonner<TestReasonner>();
	arthur.reason<TestReasonner>();

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

	sim.step_n(500, 1.0f);
	sim.executor.wait_for_all();

	return 0;
}