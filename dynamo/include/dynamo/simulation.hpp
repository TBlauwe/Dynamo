#ifndef DYNAMO_SIMULATION_HPP
#define DYNAMO_SIMULATION_HPP

#include <spdlog/fmt/bundled/format.h>

#include <dynamo/utils/containers.hpp>
#include <dynamo/internal/archetype.hpp>
#include <dynamo/internal/core.hpp>
#include <dynamo/modules/basic_perception.hpp>
#include <dynamo/modules/basic_action.hpp>

/**
@file dynamo/Simulation.hpp
@brief Main entry point to use the library.
*/
namespace dynamo {

    /**
    @class Simulation

    @brief Class used to create and manipulate a simulation of agents evolving within a virtual world.
    */
    class Simulation
    {
        friend class DynamoInspector;
    public:
        /**
        @brief Construct an empty simulation.
        */
        Simulation();

        /**
        @brief Construct an empty simulation. Also set the number of threads (default: std::thread::hardware_concurrency).
        */
        Simulation(size_t number_of_threads);

        /**
        @brief Destroy the simulation. Ensure all threads are finished.
        */
        void shutdown();

        //TODO add constraint
        /**
        @brief Register a flow builder so that it can be instantiaed for each relevant agent and executed when required.
        @tparam Must be a callable of type std::function<void(Agent)>. /!\ Not enforced ! /!\
        */
        template<typename T>
        void flow()
        {
            /**
            When an @c AddFlow<T> is added and if the parent is not a prefab (to circumvent copying),
            then create a child entity containing @c Process with T

            Since a taskflow is not copyable and unique for each agent, we must create one for each. We could add it
            as a component to an agent, but we need to set relation between them. We could still do it but the component,
            would be recreated. It seems preferable to set a child entity containing the taskflow so we can have as many
            processes and relation between them for more control.
            */
            _world.observer<const AddFlow<T>>(fmt::format("AddFlow_{}", typeid(T).name()).c_str())
                .event(flecs::OnAdd)
                .iter([this](flecs::iter& it, const AddFlow<T>* details)
                    {
                        for (auto i : it)
                        {
                            auto agent_entity   = it.entity(i);
                            if (agent_entity.has(flecs::Prefab))
                                return; // No reason to add a flow for a prefab

                            auto flow = T(&strategies, AgentHandle(agent_entity));
                            flow.build();

                            auto flow_entity = it.world().entity()
                                .set_name(flow.name())
                                //  .set<ProcessDetails>({ flow.process_details() }); // TODO Isn't this a problem with the move below ?
                                .set<Flow>({ std::move(flow) })
                                .add<Counter>()
                                .add<Duration>();

                            auto params = details[i];
                            if (params.is_cyclic)
                                flow_entity.set<Cyclic>({ params.period });

							agent_entity.remove<AddFlow<T>>();
                        }
                    }
            );

            _world.system<Flow, const Cyclic>()
                .term<Status>().oper(flecs::Not)
                .term<Cooldown>().oper(flecs::Not)
                .kind(flecs::PostFrame)
                .iter([this](flecs::iter& it, Flow* flow, const Cyclic* cycle)
                    {
                        for (auto i : it)
                        {
							auto e = it.entity(i);
                            e.add<Timestamp>();
                            e.add<Launch>();
                            e.add<Status>();
							//e.set<Status>({
							//	executor.run(flow[i].taskflow, 
       //                             [id = e.id(), period = cycle[i].period, this]()
       //                             {
       //                                 commands_queue.push([id, period](flecs::world& world) mutable {
							//				flecs::entity(world, id).remove<Status>();
							//				flecs::entity(world, id).set<Cooldown>({period});
							//				});
							//		})
							//});
                        }

                    }
            );

            _world.observer<const Status>()
                .event(flecs::OnSet)
                .each([this](flecs::entity e, const Status& process)
                    {
                        e.get_mut<Counter>()->value++;
                    }
            );

            _world.observer<const Status>()
                .event(flecs::OnRemove)
                .each([this](flecs::entity e, const Status& process)
                    {
                        e.get_mut<Duration>()->value += std::chrono::system_clock::now() - e.get<Timestamp>()->value;
                    }
            );
        };

        /**
        @brief Iterate over all agents for_each.

        @tparam T Accept function with following signature : @c std::function<void(flecs::entity, const Agent)>

        TODO : Add constraint

        Usage :
        @code{.cpp}
        sim.for_each([](flecs::entity e, const type::Agent agent){
            //...
        });
        @endcode
        */
        template<typename T>
        void for_each(T&& func)
        {
            agents_query.each(std::forward<std::function<void(flecs::entity, const type::Agent)>>(func));
        }

        /**
        @brief Iterate over all agents using a for_each.

        @tparam T Accept function with following signature : @c std::function<void(flecs::iter&)>

        TODO : Add constraint

        Usage :
        @code{.cpp}
        sim.agents_iter([](flecs::iter& iter){
            for(auto i : iter){
                //...
            }
        });
        @endcode
        */
        template<typename T>
        void agents_iter(T&& func)
        {
            agents_query.iter(std::forward<std::function<void(flecs::iter&)>>(func));
        }

        /**
        @brief Construct an action entity with specified name.
        */
        Action action(const char* name = "");

        /**
        @brief Construct an agent entity with specified name.
        */
        Agent agent(const char* name = "");

        /**
        @brief Construct an agent from the specified archetype and with specified name.
        */
        Agent agent(AgentArchetype& archetype, const char* name = "");

        /**
        @brief Construct an agent archetype with specified name.
        */
        AgentArchetype agent_archetype(const char* name = "");

        /**
        @brief Construct an agent archetype with specified name from an other archetype.
        */
        AgentArchetype agent_archetype(AgentArchetype& archetype, const char* name = "");

        /**
        @brief Construct an artefact entity with specified name.
        */
        Artefact artefact(const char* name = "");

        /**
        @brief Construct a percept entity with the specified name and the specified source.
        @param source        From which entity this percept comes from ?
        @tparam TSense       Which sense is responsible for perceiving this percept ?
        */
        template<typename TSense>
        Percept percept(flecs::entity source)
        {
            return  PerceptBuilder(_world).source<TSense>(source);
        };

        /**
        @brief Advance simulation by one-step and specify elapsed time. Return false, if application should quit.
        @param elapsed_time time elapsed. If 0 (default), then it is automatically measured;
        */
        bool step(float elapsed_time = 0.0f);

        /**
        @brief Advance simulation by @c n step and specify elapsed time between each step.
        @param n number of steps
        @param elapsed_time time elapsed. If 0 (default), then it is automatically measured;
        */
        void step_n(unsigned int n = 0, float elapsed_time = 0.0f);

        /**
        @brief Return number of commands left in the queue. Warning lock the queue so it's costly to check !.
        */
        inline size_t commands_queue_size() { return commands_queue.size(); }

        /**
        @brief Return a ref to world - an ecs "database".
        */
        flecs::world& world();

        /**
        @brief Add a new strategy of type @c T. Only one strategy of a specific type can be added.
        Multiple calls of the same type will result in undefined behaviour.

        @tparam Strategy type. Must be @c DefaultConstructible.
        */
        template<class T>
        T& strategy()
        {
            return strategies.add<T>();
        }

    private:
        void pop_commands_queue();
        void flush_commands_queue();
        void flush_for_commands_queue();

    private:
        /**
        @brief Thanks to taskflow, we used this library to incorporate task programming for our cognitive reasonning.

        For more information, see https://taskflow.github.io/
        */
        tf::Executor    executor;

        /**
        @brief ECS Database.

        For more information, see https://flecs.docsforge.com/master/quickstart/#world.
        */
        flecs::world    _world{};

        /**
        @brief Query to iterate over all agents

        For more information, see https://flecs.docsforge.com/master/quickstart/#query .
        */
        flecs::query<const type::Agent> agents_query;

        /**
        @brief Query to iterate over all taskflows that need to be launched 

        For more information, see https://flecs.docsforge.com/master/quickstart/#query .
        */
        flecs::query<Flow, Status, const Cyclic, const Launch> flows;

        /**
        @brief Defer modification to entities to a command queue called after the end of frame.
        */
        CommandsQueue commands_queue{};

        /**
        @brief Associative container to store strategies by their types. So only one strategy of a same type can be defined.
        */
        Strategies strategies;
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
    void add_tag_to(flecs::world& world, const char* tag_name, const char* term_name)
    {
        world.observer<const TTerm>(fmt::format("OnAdd_{}_Add{}", term_name, tag_name).c_str())
            .event(flecs::OnAdd)
            .each([](flecs::entity e, const TTerm& term)
                {
                    e.set<TTag>({});
                }
        );
    }
}//namespace dynamo

#endif //DYNAMO_SIMULATION_HPP
