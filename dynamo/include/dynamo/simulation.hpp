#ifndef DYNAMO_SIMULATION_HPP
#define DYNAMO_SIMULATION_HPP

#include <dynamo/internal/core.hpp>
#include <dynamo/modules/basic_perception.hpp>
#include <dynamo/internal/process.hpp>
#include <spdlog/fmt/bundled/format.h>

/**
@file dynamo/Simulation.hpp
@brief Main entry point to use the library.
*/
namespace dynamo{

    /**
    @class Simulation 

    @brief Class used to create and manipulate a simulation of agents evolving within a virtual world.
    */
    class Simulation{
    public:

        /**
        @brief Iterate over all agents for_each.

        Usage :
        @code{.cpp}
        sim.for_each([](flecs::entity e, type::Agent& agent){
            //...
        });
        @endcode
        */
        void for_each(std::function<void(flecs::entity, type::Agent&)>);

        /**
        @brief Iterate over all agents using a for_each.

        Usage :
        @code{.cpp}
        sim.agents_iter([](flecs::iter& iter){
            for(auto i : iter){
                //...
            }
        });
        @endcode
        */
        void agents_iter(std::function<void(flecs::iter&)>);

    public:
        /**
        @brief Construct an empty simulation.
        */
        Simulation();

        //TODO add constraint
        /**
        @brief Register a reasonner so that it can be executed when added to an agent.
        @tparam Must be a callable of type std::function<void(Agent)>
        */
        template<typename T>
        void register_reasonner() {
			_world.observer<component::Process<T>>()
					.event(flecs::OnAdd)
					.each([this](flecs::entity e, component::Process<T>& _){
						executor.run(T(Agent(e)));
					});
        };

        /**
        @brief Construct an agent entity with specified name.
        */
        Agent agent(const char * name = "");

        /**
        @brief Construct an artefact entity with specified name.
        */
        Artefact artefact(const char * name = "");

        /**
        @brief Construct a percept entity with the specified name and the specified source.
        @param source        From which entity this percept comes from ?
        @tparam TSense       Which sense is responsible for perceiving this percept ?
        */
        template<typename TSense>
        Percept percept(flecs::entity source){
            return  PerceptBuilder(_world).source<TSense>(source);
        };

        /**
        @brief Advance simulation by one-step and specify elapsed time.
        @param elapsed_time time elapsed. If 0 (default), then it is automatically measured;
        */
        void step(float elapsed_time = 0.0f);

        /**
        @brief Advance simulation by @c n step and specify elapsed time between each step.
        @param n number of steps
        @param elapsed_time time elapsed. If 0 (default), then it is automatically measured;
        */
        void step_n(unsigned int n = 0, float elapsed_time = 0.0f);

        /**
        @brief Return a ref to world - an ecs "database".
        */
        flecs::world& world();

    public:
        /**
        @brief Thanks to taskflow, we used this library to incorporate task programming for our cognitive reasonning.

        For more information, see https://taskflow.github.io/
        */
        tf::Executor    executor {};

    private:
        /**
        @brief ECS Database.
        
        For more information, see https://flecs.docsforge.com/master/quickstart/#world.
        */
        flecs::world    _world {};

        /**
        @brief Query to iterate over all agents

        For more information, see https://flecs.docsforge.com/master/quickstart/#query . 
        */
        flecs::query<type::Agent> agents_query;
    };

    /**
    @brief Helper function to add a tag @c TTag, each time an entity with @c TTerm is added.

    @tparam TTag Tag to add to TTerm
    @tparam TTerm Trigger when this term is added.
    @param world  Ref to world
    @param tag_name Human readable name for the tag (for debugging)
    @param term_name Human readable name for the term (for debugging)
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
