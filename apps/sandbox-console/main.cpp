#include <dynamo/dynamo.hpp>
#include <iostream>

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

using namespace dynamo;
int main(int argc, char** argv) {
    Simulation dynamo;
    dynamo.run();

    dynamo.world.system<Position, Velocity>()
            .iter([](flecs::iter it, Position *p, Velocity *v) {
                for (auto i : it) {
                    p[i].x += v[i].x * it.delta_time();
                    p[i].y += v[i].y * it.delta_time();
                }
            });

    /* Create system that is invoked once per second */
    dynamo.world.system<Position>()
            .interval(1.0)
            .each([](flecs::entity e, Position& p) {
                spdlog::info("[{}] : {},{}", e.name(), p.x, p.y);
            });

    dynamo.world.entity("Arthur")
            .set([](Position& p, Velocity& v) {
                p = {10, 20};
                v = {1, 2};
            });

    dynamo.world.set_target_fps(60);
    while (dynamo.world.progress()) {}

    dynamo.shutdown();
    return 0;
}