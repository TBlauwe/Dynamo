#ifndef DYNAMO_SIMULATION_HPP
#define DYNAMO_SIMULATION_HPP

#include <dynamo/internal/core.hpp>
#include <dynamo/modules/basic_perception.hpp>
#include <spdlog/fmt/bundled/format.h>

namespace dynamo{

    /**
     * Simulation of a number of agents evolving inside a world.
     */
    class Simulation{
    private:
        /**
         * ECS Database. For more information, see https://flecs.docsforge.com/master/quickstart/#world.
         */
        flecs::world    _world {};
    public:
        tf::Executor    executor {};

    public:
        /**
         * Query for all agents :
         * Usage :
         *  @code
         *      agents_query.each([](flecs::entity e, type::Agent& agent){
         *          //...
         *      };
         *
         *      agents_query.iter([](flecs::iter& iter){
         *          for(auto i : iter){
         *              //...
         *          }
         *      };
         *  @endcode
         */
        flecs::query<type::Agent> agents_query;

    public:
        /**
         * Create an empty simulation.
         */
        Simulation();

        /**
         * Create a ready to use "Agent" entity with the specified name.
         * @param name Handle for this entity.
         */
        TypeHandler<type::Agent> agent(const char * name = "");

        /**
         * Create a ready to use "Artefact" entity with the specified name.
         * @param name Handle for this entity.
         */
        TypeHandler<type::Artefact> artefact(const char * name = "");

        /**
         * Create a ready to use "Percept" entity with the specified name and the specified source.
         * @param source        From which entity this percept comes from ?
         * @tparam TSense       Which sense is responsible for perceiving this percept ?
         * @return
         */
         template<typename TSense>
        TypeHandler<type::Percept> percept(flecs::entity source){
            return  TypeBuilder<type::Percept>(_world).source<TSense>(source);
        };


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

    /**
     * Helper function to add a tag TTag, each time an entity with TTerm is added.
     * @tparam TTag Tag to add to TTerm
     * @tparam TTerm Trigger when this term is added.
     * @param world  Ref to world
     * @param tag_name Human readable name for the tag (for debugging)
     * @param term_name Human readable name for the term (for debugging)
     */
    template<typename TTag, typename TTerm>
    void add_tag_to(flecs::world& world, const char * tag_name, const char * term_name){
        world.system<const TTerm>(fmt::format("OnAdd_{}_Add{}", term_name, tag_name).c_str())
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const TTerm& term){
                    e.set<TTag>({});
                });
    }
}//namespace dynamo

#endif //DYNAMO_SIMULATION_HPP
