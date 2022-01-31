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

    @brief Wrap an entity for convenience.
    */
    class EntityWrapper
    {
    public:
        /**
        @brief consider the given entity as a @c Type.

        We can't check if entity has the corresponding @c Type tag as it may be
        staged.
        */
        EntityWrapper() : m_entity{} {};
        explicit EntityWrapper(flecs::entity entity) : m_entity{ entity } {};

        friend bool operator== (const EntityWrapper& a, const EntityWrapper& b);
        /**
        @brief Returns entity's name. /!\ Can be null (most likely for percepts
        entities) !

        For more information see : https://flecs.docsforge.com/master/manual/#entity.
        */
        const char* name() const { return m_entity.name(); }

        /**
        @brief Returns @c true or @ false, if has given component.
        @tparam TType Component's type.
        */
        template<typename TType>
        bool has() const { return m_entity.has<TType>(); }

        /**
        @brief Returns a pointer to the const component.
        @tparam TType Component's type.
        */
        template<typename TType>
        TType const* get() const { return m_entity.get<TType>(); }

        /**
        @brief Returns a pointer to the component.
        @tparam TType Component's type.

        if you need to modify a component, prefer using set function as it will defer modification when possible. Otherwise, if you use
        this pointer to modify the component while the world is in read-only (assume always when in a process), it will crash.
        */
        template<typename TType>
        TType* get_mut() { return m_entity.get_mut<TType>(); }

        /**
        @brief Returns the underlying entity.

        For more information see : https://flecs.docsforge.com/master/manual/#entity.
        */
        inline flecs::entity entity() const { return m_entity; }

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

    inline bool operator== (const EntityWrapper& a, const EntityWrapper& b)
    { 
        return a.m_entity.id() == b.m_entity.id();
    }

    /**
    @class EntityManipulator

    @brief Derived from @c EntityWrapper to expose underliying call to modify/query the state of an entity.

    @tparam T Type of the derived class so we can expose a fluent API.
    */
    template<typename T>
    class EntityManipulator : public EntityWrapper
    {
    public:
        using EntityWrapper::EntityWrapper;

        /**
        @brief Add a component (with default value).
        @tparam TType Component's type.
        */
        template<typename TType>
        T& add()
        {
            m_entity.add<TType>();
            return *static_cast<T*>(this);
        }

        /**
        @brief Set a component (with provided value).
        @tparam TType Component's type.
        */
        template<typename TType>
        T& set(TType&& value)
        {
            m_entity.set<TType>(std::forward<TType>(value));
            return *static_cast<T*>(this);
        }

        /**
        @brief Removes a component.
        @tparam TType Component's type.
        */
        template<typename TType>
        T& remove()
        {
            m_entity.remove<TType>();
            return *static_cast<T*>(this);
        }
    private:
        /**
        @brief Test whether the world is in read-only mode or not.
        
        Sometimes an entity refers to a read-only world (especially while reasonning due to
        async processes outside conventionnal progress loop). We need to check before hand if we should defer
        or not the modification
        */
        bool should_defer()
        {
            return m_entity.world().is_readonly();
        }
    };

    /**
    @class EntityManipulator

    @brief Derived from @c EntityWrapper to expose underliying call to modify/query the state of an entity.

    @tparam T Type of the derived class so we can expose a fluent API.
    */
    template<typename T>
    class DefferedEntityManipulator : public EntityWrapper
    {
    public:
        explicit DefferedEntityManipulator(flecs::entity entity) : EntityWrapper{ entity }
        {
            commands_queue = m_entity.world().template get<type::CommandsQueueHandle>()->queue;
        };

        /**
        @brief Add a component (with default value).
        @tparam TType Component's type.
        */
        template<typename TType>
        T& add()
        {
            commands_queue->push([id = m_entity.id()](flecs::world& world) mutable {
                flecs::entity(world, id).add<TType>();
            });
            return *static_cast<T*>(this);
        }

        template<typename R, typename V>
        T& add(V&& value)
        {
            commands_queue->push([id = m_entity.id(), args = std::forward<V>(value)](flecs::world& world) mutable {
                flecs::entity(world, id).add<R>(args);
            });
            return *static_cast<T*>(this);
        }

        /**
        @brief Set a component (with provided value).
        @tparam TType Component's type.
        */
        template<typename TType>
        T& set(TType&& value)
        {
            commands_queue->push([id = m_entity.id(), args = std::forward<TType>(value)](flecs::world& world) mutable {
                flecs::entity(world, id).set<TType>(args);
            });
            return *static_cast<T*>(this);
        }

        /**
        @brief Removes a component.
        @tparam TType Component's type.
        */
        template<typename TType>
        T& remove()
        {
            commands_queue->push([id = m_entity.id()](flecs::world& world) mutable {
                auto entity = flecs::entity(world, id);
                if (entity.has<TType>())
                    entity.remove<TType>();
            });
            return *static_cast<T*>(this);
        }

    private:
        /**
        @brief Pointer to simulations' commands queue to defer modifications when world is in read only.
        */
        type::CommandsQueue* commands_queue;
    };

    /**
    @class 
    @brief Convenience class if we do not know the type of the entity.
    */
    class Entity : public EntityManipulator<Entity> {
    public:
        Entity() : EntityManipulator<Entity>() {};
        /**
        @brief Identify the given entity as an @c Entity
        */
        explicit Entity(flecs::entity entity) : EntityManipulator<Entity>(entity) {};
    };

    /**
    @class Action

    @brief Action are entities. WIP
    */
    class Action : public EntityManipulator<Action> {
    public:
        Action() : EntityManipulator<Action>() {};
        /**
        @brief Identify the given entity as an @c Action. Must have the corresponding
        tag.

        To construct an @c Action , see @c ActionBuilder .
        */
        explicit Action(flecs::entity entity) : EntityManipulator<Action>(entity) {};
    };

    // Forward declaration
    class Reasonner;

    /**
    @class Agent

    @brief Agent are pro-active entities with cognitive abilities.
    */
    class Agent : public EntityManipulator<Agent> {
    public:
        /**
        @brief Identify the given entity as an @c Agent. Must have the corresponding
        tag.

        To construct an @c Agent , see @c AgentBuilder .
        */
        explicit Agent(flecs::entity entity) : EntityManipulator<Agent>(entity) {};

        std::vector<std::string> agent_models()
        {
            std::vector<std::string> models{};
            m_entity.children([&models](flecs::entity e) 
                {
                    if (e.has<type::ProcessHandle>())
                    {
                        models.emplace_back(e.get<type::ProcessHandle>()->taskflow->dump());
                    }
                }
            );
            return models;
        }
        
        template<typename T>
        void for_each_children(T&& func)
        {
            m_entity.children(std::forward<std::function<void(flecs::entity)>>(func));
        }

        void cancel_all_reasonning()
        {
            m_entity.children([](flecs::entity child) {
                if (child.has<type::IsProcessing>())
                {
                    auto* p = child.get_mut<type::IsProcessing>();
                    p->status.cancel();
                }
            });
        }
    };

    class AgentHandle : public DefferedEntityManipulator<AgentHandle>
    {
    public:
        /**
        @brief Handle for manipulating agent where every modifications are deferred.
        */
        explicit AgentHandle(flecs::entity entity) : DefferedEntityManipulator<AgentHandle>(entity) {};
    };


    /**
    @class Artefact

    @brief Artefact are passive entities.
    */
    class Artefact : public EntityManipulator<Artefact> {
    public:
        /**
        @brief Identify the given entity as an @c Artefact. Must have the
        corresponding tag.

        To construct an @c Artefact , see @c ArtefactBuilder .
        */
        explicit Artefact(flecs::entity entity) : EntityManipulator<Artefact>(entity) {};
    };

    /**
    @class Organisation

    @brief Organisation are intangible entities that other entities can belongs to.
    */
    class Organisation : public EntityManipulator<Organisation> {
    public:
        /**
        @brief Identify the given entity as an @c Organisation. Must have the
        corresponding tag.

        To construct an @c Organisation , see @c OrganisationBuilder .
        */
        explicit Organisation(flecs::entity entity)
            : EntityManipulator<Organisation>(entity) {};
    };

    /**
    @class Percept

    @brief Percept are tangible entities that other entities can perceive.

    A @Percept is coming from an other entity, identity as a source, and can be
    perceived by other entities, specified by @ref perceived_by(flecs::entity).

    A decay can also be specified to delete @Percept after some time. See @ref
    decay(float ttl).
    */
    class Percept : public EntityManipulator<Percept> {
    public:
        /**
        @brief Identify the given entity as an @c Percept. Must have the corresponding
        tag.

        To construct an @c Percept , see @c PerceptBuilder .
        */
        explicit Percept(flecs::entity entity) : EntityManipulator<Percept>(entity) {};

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
