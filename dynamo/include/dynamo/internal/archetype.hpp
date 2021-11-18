#ifndef DYNAMO_ARCHETYPE_HPP
#define DYNAMO_ARCHETYPE_HPP

#include <dynamo/internal/process.hpp>

namespace dynamo
{
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
        AgentArchetype& agent_model()
        {
            static_assert(std::is_base_of<AgentModel, T>::value, "Wrong type passed : T is not inheriting from AgentModel.");

            auto reasonnner_entity = m_entity.world().entity();
            reasonnner_entity
                .child_of(m_entity)
                .add<type::AddProcess<T>>()
                ;

            return *this;
        }
    };
}  // namespace dynamo
#endif  // DYNAMO_ARCHETYPE_HPP
