#include <dynamo/simulation.hpp>
#include <iostream>

using namespace dynamo;
int main(int argc, char** argv) {

    // Create an empty simulation
    Simulation sim;

    //TODO Transivity seems to not be supported by observer. Test with query !
    sim.world().observer<>()
            .term<type::Agent>()
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

    return 0;
}