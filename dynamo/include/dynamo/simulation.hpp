#ifndef DYNAMO_SIMULATION_HPP
#define DYNAMO_SIMULATION_HPP

#include <dynamo/modules/core.hpp>
#include <dynamo/modules/basic_perception.hpp>

namespace dynamo{

    /**
     * Simulation of a number of agents evolving inside a world.
     */
    class Simulation{
    private:
        flecs::world    _world {};

    public:
        /**
         * Create an empty simulation.
         */
        Simulation();

        /**
         * Create a ready to use "Agent" entity with the specified name.
         * @param name Handle for this entity.
         */
        flecs::entity agent(const char * name = "");

        /**
         * Create a ready to use "Artefact" entity with the specified name.
         * @param name Handle for this entity.
         */
        flecs::entity artefact(const char * name = "");

        /**
         * Create a ready to use "Percept" entity with the specified name and the specified source.
         * @param source    From which entity this percept comes from ?
         * @param ttl       How much time should this percept stay alive ? Default: 2.0f
         * @param name      Identifier (can be empty).
         * @return
         */
        flecs::entity percept(flecs::entity source, float ttl = 2.0f, const char * name = "");

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

#endif //DYNAMO_SIMULATION_HPP
