#include <dynamo/simulation.hpp>
#include <iostream>

#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

using namespace dynamo;

int main(int argc, char** argv) {

    // Create an empty simulation
    Simulation sim;

    sim.world().observer<>()
            .term<::dynamo::type::Agent>()
            .term<relation::perceive>(flecs::Wildcard)
            .event(flecs::OnAdd)
            .iter([](flecs::iter& iter){
                      std::cout << "Triggered \n";
                      auto object = iter.term_id(2).object();
                      for (auto i : iter) {
                          std::cout << "Entity      : " << iter.entity(i).name() << "\n";
                          std::cout << "Object      : " << object.name() << "\n";
                          std::cout << "Object Type : " << object.type().str() << "\n";
                      }
                  }
            );

    auto arthur = sim.agent("Arthur");
    auto bob = sim.agent("Bob");
    auto radio = sim.artefact("Radio");
    sim.percept<senses::Hearing>(radio.entity())
            .perceived_by(radio.entity())
            .perceived_by(bob.entity())
            .perceived_by(arthur.entity())
            ;

    tf::Taskflow f1;
    f1.name("Some");

    // create taskflow f1 of two tasks
    tf::Task f1A = f1.emplace([](){}).name("f1A");
    tf::Task f1B = f1.emplace([](){}).name("f1B");

    tf::Taskflow taskflow;
    taskflow.name("Test");
    tf::Task A = taskflow.emplace([](){}).name("A");
    tf::Task C = taskflow.emplace([](){}).name("C");
    tf::Task D = taskflow.emplace([](){}).name("D");

    tf::Task B = taskflow.emplace([] (tf::Subflow& subflow) {
        tf::Task B1 = subflow.emplace([](){}).name("B1");
        tf::Task B2 = subflow.emplace([](){}).name("B2");
        tf::Task B3 = subflow.emplace([](){}).name("B3");
        B3.succeed(B1, B2);  // B3 runs after B1 and B2
    }).name("B");

    taskflow.emplace([](){}).name("lonely");

    tf::Task module = taskflow.composed_of(f1).name("module");
    A.succeed(module);
    A.precede(B, C);  // A runs before B and C
    D.succeed(B, C);  // D runs after  B and C
    sim.executor.run(taskflow);

    taskflow.for_each_task([](tf::Task task){
        switch(task.type()){
            case tf::TaskType::MODULE:
                std::cout << "[" << to_string(task.type()) << "] " << task.name() << "\n";
                break;
            case tf::TaskType::PLACEHOLDER:
            case tf::TaskType::CUDAFLOW:
            case tf::TaskType::SYCLFLOW:
            case tf::TaskType::STATIC:
            case tf::TaskType::DYNAMIC:
            case tf::TaskType::CONDITION:
            case tf::TaskType::ASYNC:
            case tf::TaskType::UNDEFINED:
            default:
                std::cout << "[" << to_string(task.type()) << "] " << task.name() << "\n";
                break;
        }
    });

    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
                       ogdf::GraphAttributes::nodeGraphics |
                       ogdf::GraphAttributes::edgeGraphics |
                       ogdf::GraphAttributes::nodeLabel |
                       ogdf::GraphAttributes::edgeStyle |
                       ogdf::GraphAttributes::nodeStyle |
                       ogdf::GraphAttributes::nodeTemplate);

    // Layout
    for (ogdf::node v : G.nodes)
        GA.width(v) = GA.height(v) = 5.0;

    ogdf::SugiyamaLayout SL;
    SL.setRanking(new ogdf::OptimalRanking);
    SL.setCrossMin(new ogdf::MedianHeuristic);

    auto *ohl = new ogdf::OptimalHierarchyLayout;
    ohl->layerDistance(30.0);
    ohl->nodeDistance(25.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);

    SL.call(GA);
    sim.executor.wait_for_all();

    return 0;
}