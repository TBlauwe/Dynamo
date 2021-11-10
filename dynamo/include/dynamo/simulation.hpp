#ifndef DYNAMO_SIMULATION_HPP
#define DYNAMO_SIMULATION_HPP

#include <spdlog/fmt/bundled/format.h>

#include <dynamo/internal/core.hpp>
#include <dynamo/modules/basic_perception.hpp>
#include <dynamo/internal/process.hpp>

/**
@file dynamo/Simulation.hpp
@brief Main entry point to use the library.
*/
namespace dynamo {

    /**
    @class Archetype
    @brief Abstract class to define prefab for easily instantiate entity with a set of components.

    @tparam Type of the inheriting class so that the correct class can be returned without breaking fluent api.
    */
    template<typename TBuilder>
    class Archetype : public EntityWrapper
    {
    public:
        /**
        @brief Construct an empty prefab.
        */
        Archetype(flecs::world& world, const char* name) : EntityWrapper{ world.prefab(name) } {}

        /**
        @brief Construct a prefab inheriting an other prefab.
        */
        Archetype(flecs::world& world, flecs::entity prefab, const char* name) : EntityWrapper{ world.prefab(name).is_a(prefab)} {}

        /**
        @brief Add a component (with default value).
        @tparam TType Component's type.
        */
        template<typename TType>
        TBuilder& add()
        {
            m_entity.override<TType>();
            return *static_cast<TBuilder*>(this);
        }

        /**
        @brief Add a shared component between all instances of this prefab (with default value).
        @tparam TType Component's type.
        */
        template<typename TType>
        TBuilder& add_shared()
        {
            m_entity.add<TType>();
            return *static_cast<TBuilder*>(this);
        }

        /**
        @brief Add and set a shared component between all instances of this prefab.
        @tparam TType Component's type.
        */
        template<typename TType>
        TBuilder& set(TType&& value)
        {
            m_entity.set<TType>(value);
            m_entity.override<TType>();
            return *static_cast<TBuilder*>(this);
        }

        /**
        @brief Add and set a component.
        @tparam TType Component's type.
        */
        template<typename TType>
        TBuilder& set_shared(TType&& value)
        {
            m_entity.set<TType>(value);
            return *static_cast<TBuilder*>(this);
        }
    };

    /**
    @class AgentArchetype
    @brief Class used to define agents' prefabs for easily instantiate an agent according to an archetype.

    @code{.cpp}
    Simulation sim;

    // Define some data ..

    auto archetype = sim.agent_archetype()
        .add<...>()
        .add_shared<...>()
        .set<...>(value...)
        .set_shared<...>(value...);

    sim.agent(archetype, "my_new_agent");

    @endcode

    Also, you can derive an archetype to make an other archetype

    @code{.cpp}
    Simulation sim;

    // Define some data ..

    auto archetype = sim.agent_archetype()
        .add<...>()
        .add_shared<...>()
        .set<...>(value...)
        .set_shared<...>(value...);

    sim.agent(archetype, "my_new_agent");

    auto another_archetype = sim.agent_archetype(archetype)
        .add<...>();

    sim.agent(another_archetype, "my_other_new_agent");

    @endcode
    */
    class AgentArchetype : public Archetype<AgentArchetype>
    {
    public:
        AgentArchetype(flecs::world& world, const char* name) : Archetype<AgentArchetype>{ world, name } {}
        AgentArchetype(flecs::world& world, AgentArchetype& archetype, const char* name) : Archetype<AgentArchetype>{ world, archetype, name } {}

        template<typename T>
        AgentArchetype& add_reasonner()
        {
            static_assert(std::is_base_of<Reasonner, T>::value, "Wrong type passed : T is not inheriting from Reasonner.");

            auto reasonnner_entity = m_entity.world().entity();
            reasonnner_entity
                .child_of(m_entity)
                .add<type::AddProcess<T>>()
                ;

            return *this;
        }
    };

    /**
    @class Simulation

    @brief Class used to create and manipulate a simulation of agents evolving within a virtual world.
    */
    class Simulation
    {
    public:
        /**
        @brief Construct an empty simulation.
        */
        Simulation();

        //TODO add constraint
        /**
        @brief Register a reasonner so that it can be instantiaed for each relevant agent and executed when required.
        @tparam Must be a callable of type std::function<void(Agent)>. /!\ Not enforced ! /!\
        */
        template<typename T>
        void register_reasonner()
        {
            /**
            When an @c type::AddProcess<T> is added and if the parent is not a prefab (to circumvent copying),
            then create a child entity containing @c type::Process with T

            Since a taskflow is not copyable and unique for each agent, we must create one for each. We could add it
            as a component to an agent, but we need to set relation between them. We could still do it but the component,
            would be recreated. It seems preferable to set a child entity containing the taskflow so we can have as many
            processes and relation between them for more control.
            */
            _world.observer<type::AddProcess<T>>()
                .event(flecs::OnAdd)
                .each([](flecs::entity e, type::AddProcess<T>& _)
                    {
                        auto parent = e.get_object(flecs::ChildOf);
                        if (parent.has(flecs::Prefab))
                            return;
                        // By construction, only entity agent can be a parent of these entities (except for the prefab we checked earlier)
                        e.set<type::Process>({T(Agent(parent))});
                        e.remove<type::AddProcess<T>>();
                    }
            );

            _world.system<type::Process>()
                .term<type::IsProcessing>().oper(flecs::Not)
                .kind(flecs::OnUpdate)
                .each([this](flecs::entity e, type::Process& process)
                    {
                        std::cout << "Launching reasonning for " << e.get_object(flecs::ChildOf).name() << std::endl;
                        executor.run(process.taskflow).wait();
                        e.set<type::IsProcessing>({ executor.run(std::move(process.taskflow)) });
                    }
            );

            _world.system<const type::Process, const type::IsProcessing>()
                .kind(flecs::PreUpdate)
                .each([this](flecs::entity e, const type::Process& p, const type::IsProcessing& process)
                    {
                        std::cout << "----------------------------------------\n";
                        std::cout << "Nb of tasks : " << p.taskflow.num_tasks() << std::endl;
                        std::cout << "Status for " << e.get_object(flecs::ChildOf).name() << " : " << static_cast<int>(process.status.wait_for(std::chrono::seconds(0))) << std::endl;
                        //e.set<type::IsProcessing>({ executor.run(process.taskflow) });
                    }
            );
        };

        /**
        @brief Iterate over all agents for_each.

        @tparam T Accept function with following signature : @c std::function<void(flecs::entity, type::Agent&)>

        TODO : Add constraint

        Usage :
        @code{.cpp}
        sim.for_each([](flecs::entity e, type::Agent& agent){
            //...
        });
        @endcode
        */
        template<typename T>
        void for_each(T&& func)
        {
            agents_query.each(std::forward<std::function<void(flecs::entity, type::Agent&)>>(func));
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
        tf::Executor    executor{};

    private:
        /**
        @brief ECS Database.

        For more information, see https://flecs.docsforge.com/master/quickstart/#world.
        */
        flecs::world    _world{};

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
    void add_tag_to(flecs::world& world, const char* tag_name, const char* term_name)
    {
        world.system<const TTerm>(fmt::format("OnAdd_{}_Add{}", term_name, tag_name).c_str())
            .kind(flecs::OnAdd)
            .each([](flecs::entity e, const TTerm& term)
                {
                    e.set<TTag>({});
                }
        );
    }
}//namespace dynamo

#endif //DYNAMO_SIMULATION_HPP
