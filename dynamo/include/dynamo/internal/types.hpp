#ifndef DYNAMO_TYPES_HPP
#define DYNAMO_TYPES_HPP

#include <iostream>

#include <flecs.h>

#include <dynamo/internal/components.hpp>
#include <dynamo/internal/relations.hpp>

/**
@file dynamo/internal/types.hpp
@brief Defines different kind of entities tags used by the library.

An entity can only be defined as one type, e.g :
"A cannot be an action and an agent at the same time".

/!\ Not enforced /!\
*/
namespace dynamo::type {

    /**
    @brief Mark entity as an action.
    */
    struct Action {};

    /**
    @brief Mark entity as an agent.
    */
    struct Agent {};

    /**
    @brief Mark entity as an artefact.
    */
    struct Artefact {};

    /**
    @brief Mark entity as an organisation.
    */
    struct Organisation {};

    /**
    @brief Mark entity as a percept .
    */
    struct Percept {};
}

namespace dynamo {
    /**
    @class EntityWrapper

    @brief Abstract class to wrap an entity for convenience.

    Wraps an entity for convience.
    */
    class EntityWrapper {
    public:
        /**
        @brief consider the given entity as a @c Type.

        We can't check if entity has the corresponding @c Type tag as it may be
        staged.
        */
        explicit EntityWrapper(flecs::entity entity) : m_entity{ entity } {};

        /**
        @brief Returns entity's name. /!\ Can be null (most likely for percepts
        entities) !

        For more information see : https://flecs.docsforge.com/master/manual/#entity.
        */
        const char* name() { return m_entity.name(); }

        /**
        @brief Returns the underlying entity.

        For more information see : https://flecs.docsforge.com/master/manual/#entity.
        */
        inline flecs::entity entity() { return m_entity; }

        /**
        @brief Implicit conversion operator to @c flecs::entity.

        For more information see : https://flecs.docsforge.com/master/manual/#entity.
        */
        inline operator flecs::entity() { return m_entity; }

        /**
        @brief Implicit conversion operator to @c flecs::id_t.

        For more information see : https://flecs.docsforge.com/master/manual/#entity.
        */
        inline operator flecs::id_t() { return m_entity; }

    protected:
        flecs::entity m_entity;
    };

    /**
    @class Action

    @brief Action are entities. WIP
    */
    class Action : public EntityWrapper {
    public:
        /**
        @brief Identify the given entity as an @c Action. Must have the corresponding
        tag.

        To construct an @c Action , see @c ActionBuilder .
        */
        explicit Action(flecs::entity entity) : EntityWrapper(entity) {};
    };

    // Forward declaration
    class Reasonner;

    /**
    @class Agent

    @brief Agent are pro-active entities with cognitive abilities.
    */
    class Agent : public EntityWrapper {
    public:
        /**
        @brief Identify the given entity as an @c Agent. Must have the corresponding
        tag.

        To construct an @c Agent , see @c AgentBuilder .
        */
        explicit Agent(flecs::entity entity) : EntityWrapper(entity) {};

        /**
        @brief Tell this agent to reason about something with @c T a @c Reasonner.

                @tparam T is the type of a reasonner that should be spawned.
        */
        template <typename T>
        void reason() {
            // static_assert(std::is_base_of<Reasonner, T>::value, "Wrong type passed,
            // must be a Reasonner.");
            // entity().set<component::Process>();
        };
    };

    /**
    @class Artefact

    @brief Artefact are passive entities.
    */
    class Artefact : public EntityWrapper {
    public:
        /**
        @brief Identify the given entity as an @c Artefact. Must have the
        corresponding tag.

        To construct an @c Artefact , see @c ArtefactBuilder .
        */
        explicit Artefact(flecs::entity entity) : EntityWrapper(entity) {};
    };

    /**
    @class Organisation

    @brief Organisation are intangible entities that other entities can belongs to.
    */
    class Organisation : public EntityWrapper {
    public:
        /**
        @brief Identify the given entity as an @c Organisation. Must have the
        corresponding tag.

        To construct an @c Organisation , see @c OrganisationBuilder .
        */
        explicit Organisation(flecs::entity entity)
            : EntityWrapper(entity) {};
    };

    /**
    @class Percept

    @brief Percept are tangible entities that other entities can perceive.

    A @Percept is coming from an other entity, identity as a source, and can be
    perceived by other entities, specified by @ref perceived_by(flecs::entity).

    A decay can also be specified to delete @Percept after some time. See @ref
    decay(float ttl).
    */
    class Percept : public EntityWrapper {
    public:
        /**
        @brief Identify the given entity as an @c Percept. Must have the corresponding
        tag.

        To construct an @c Percept , see @c PerceptBuilder .
        */
        explicit Percept(flecs::entity entity) : EntityWrapper(entity) {};

        /**
        @brief Add a relation "type::perceive" from the given entity e to this
        percept

        @param e entity perceiving this percept
        */
        Percept& perceived_by(flecs::entity e) {
            e.mut(m_entity).add<type::perceive>(m_entity);
            return *this;
        }

        /**
        @brief Add a relation "type::perceive" from the given entity e to this
        percept

        @param e entity perceiving this percept
        */
        Percept& perceived_by(flecs::entity_view e) {
            e.mut(m_entity).add<type::perceive>(m_entity);
            return *this;
        }

        /**
        @brief This percept will be destroyed after the specified amount

        @param ttl how much time should this entity live ?
        */
        Percept& decay(float ttl = 2.0f) {
            m_entity.set<type::Decay>({ ttl });
            return *this;
        }
    };

    /**
    @class Builder

    @brief Convenience builder to create an entity with the specified type.

    @tparam TTag Tad added to the entity
    */
    template <typename TTag>
    class Builder {
    public:
        /**
        @brief Construct an unnamed entity with the given tag @c TTag.
        */
        explicit Builder(flecs::world& world) : world{ world }, entity{ world.entity() } {
            entity.add<TTag>();
        };

        /**
    @brief Construct a named entity with the given tag @c TTag.
        */
        explicit Builder(flecs::world& world, const char* name)
            : world{ world }, entity{ world.entity(name) } {
            entity.add<TTag>();
        };

    protected:
        flecs::world& world;
        flecs::entity entity;
    };

    /**
    @class ActionBuilder

    @brief Convenience builder to create an action entity.
    */
    class ActionBuilder : public Builder<type::Action> {
    public:
        /**
        @brief Construct a named action entity.
        */
        explicit ActionBuilder(flecs::world& world, const char* name)
            : Builder<type::Action>(world, name) {};

        /**
        @brief Returns an @c Action with built entity.
        */
        Action build() { return Action(entity); }
    };

    /**
    @class AgentBuilder

    @brief Convenience builder to create an agent entity.
    */
    class AgentBuilder : public Builder<type::Agent> {
    public:
        /**
        @brief Construct a named agent entity.
        */
        explicit AgentBuilder(flecs::world& world, const char* name)
            : Builder<type::Agent>(world, name) {};

        /**
        @brief Returns an @c Agent with built entity.
        */
        Agent build() { return Agent(entity); }
    };

    /**
    @class ArtefactBuilder

    @brief Convenience builder to create an artefact entity.
    */
    class ArtefactBuilder : public Builder<type::Artefact> {
    public:
        /**
        @brief Construct a named artefact entity.
        */
        explicit ArtefactBuilder(flecs::world& world, const char* name)
            : Builder<type::Artefact>(world, name) {};

        /**
        @brief Returns an @c Artefact with built entity.
        */
        Artefact build() { return Artefact(entity); }
    };

    /**
    @class OrganisationBuilder

    @brief Convenience builder to create an organisation entity.
    */
    class OrganisationBuilder : public Builder<type::Organisation> {
    public:
        /**
        @brief Construct a named organisation entity.
        */
        explicit OrganisationBuilder(flecs::world& world, const char* name)
            : Builder<type::Organisation>(world, name) {};

        /**
        @brief Returns an @c Organisation with built entity.
        */
        Organisation build() { return Organisation(entity); }
    };

    /**
    @class PerceptBuilder

    @brief Convenience builder to create percepts as we need to specify its type at
    creation and its source.
    */
    class PerceptBuilder : public Builder<type::Percept> {
    public:
        /**
        @brief Construct an unnamed percept entity.
        */
        explicit PerceptBuilder(flecs::world& world)
            : Builder<type::Percept>(world) {};

        /**
        @brief Construct a percept coming from specified source with specified sense
        @TSense.

        A relation "percept->source->object" is added.

        @tparam TSense type of sense.
        @param source entity responsible for the creation of this percept.
        */
        template <typename TSense>
        Percept source(flecs::entity source) {
            return Percept(entity.add<type::source>(source)
                .add<type::perceive>(source)
                .add<TSense>());
        }
    };
}  // namespace dynamo
#endif  // DYNAMO_TYPES_HPP
