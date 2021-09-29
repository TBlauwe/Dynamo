#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation(flecs::world& world) {
    world.module<Simulation>();
    auto e = world.import<module::Core>();
    logger(world)->info("Loading module : Simulation");

}