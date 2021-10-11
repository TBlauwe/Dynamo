#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation() {
    world.import<module::Core>();
    world.import<module::GlobalPerception>();
}

void dynamo::Simulation::step(){
    world.progress();
}

void dynamo::Simulation::step_n(unsigned int n) {
    for(int i {0}; i < n; i++){
        step();
    }
}
