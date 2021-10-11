#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <dynamo/modules/core.hpp>
#include <dynamo/modules/basic_perception.hpp>

namespace dynamo{

    /**
     * Simulation of a number of agents evolving inside a world.
     */
    class Simulation{
    private:
        flecs::world _world {};

    public:
        /**
         * Create an empty simulation.
         */
        Simulation();

        /**
         * Advance simulation by one-step and specify elapsed time.
         * @param elapsed_time time elapsed. If 0 (default), then it is automatically measured;
         */
        void step(float elapsed_time = 0.0f);

        /**
         * Advance simulation by n-step and specify elapsed time between each step
         * @param n number of steps
         * @param elapsed_time time elapsed. If 0 (default), then it is automatically measured;
         */
        void step_n(unsigned int n = 0, float elapsed_time = 0.0f);

        /**
         * @return world - an ecs "database".
         */
        flecs::world& world();
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
