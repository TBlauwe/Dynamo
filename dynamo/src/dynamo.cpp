#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation() {
    _world.import<module::Core>();
    _world.import<module::GlobalPerception>();
}

void dynamo::Simulation::step(){
    _world.progress();
}

void dynamo::Simulation::step_n(unsigned int n) {
    for (int i{0}; i < n; i++) {
        step();
    }
}

flecs::world &dynamo::Simulation::world() {
    return _world;
}
