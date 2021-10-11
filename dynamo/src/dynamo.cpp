#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation(flecs::world& world) {
    world.module<Simulation>();
    world.import<module::Core>();
    world.import<module::GlobalPerception>();
}