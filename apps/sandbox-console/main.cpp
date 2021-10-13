#include <dynamo/simulation.hpp>
#include <iostream>

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

struct Default {};

using namespace dynamo;
int main(int argc, char** argv) {
    Simulation sim;

    sim.world().system<Position, Velocity>()
            .iter([](flecs::iter it, Position *p, Velocity *v) {
                for (auto i : it) {
                    p[i].x += v[i].x * it.delta_time();
                    p[i].y += v[i].y * it.delta_time();
                }
            });

    /* Create system that is invoked once per second */
    sim.world().system<Position>()
            .interval(1.0)
            .each([](flecs::entity e, Position& p) {
                std::cout << "Entity : " << e.name() << " | Position : {" << p.x << "," << p.y << "}\n";
            });

    auto agent = sim.agent("Arthur").entity()
            .set([](Position& p, Velocity& v) {
                p = {0, 0};
                v = {1, 2};
            });

    auto artefact = sim.artefact("Radio").entity();

    auto percept = sim.percept<Default>(artefact);

    sim.world().set_target_fps(60);
    sim.step_n(100, 1.0f);

    // Arthur position won't be multiplied by 2 !
    // It should take very few seconds to run 100 iterations (1 sec for example).
    sim.step_n(100);

    return 0;
}