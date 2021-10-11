#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <dynamo/modules/core.hpp>
#include <dynamo/modules/basic_perception.hpp>

namespace dynamo{

    /**
     * Simulation of a number of agents evolving inside a world.
     */
    class Simulation{
        flecs::world _world {};

    public:
        /**
         * Create an empty simulation.
         */
        Simulation();

        /**
         * Advance simulation by one-step.
         */
        void step();

        /**
         * Advance simulation by n-step.
         * @param n number of steps
         */
        void step_n(unsigned int n = 0);

        /**
         * @return world - an ecs "database".
         */
        flecs::world& world();
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
