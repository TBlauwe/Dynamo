#ifndef DYNAMO_TYPES_HPP
#define DYNAMO_TYPES_HPP

#include <flecs.h>
#include <dynamo/internal/relations.hpp>
#include <dynamo/internal/components.hpp>

namespace dynamo {

    /**
     * Refers to a kind of entities that are mutually exclusive (an agent cannot be an organisation, etc.).
     */
    namespace type {
        /**
         * Mark entity as an action.
         */
        struct Action {
        };

        /**
         * Mark entity as an agent.
         */
        struct Agent {
        };

        /**
         * Mark entity as an artefact.
         */
        struct Artefact {
        };

        /**
         * Mark entity as an organisation.
         */
        struct Organisation {
        };

        /**
         * Mark entity as a percept .
         */
        struct Percept {
        };
    }

    /**
     * Entity wrapper (for convenience).
     */
    class EntityWrapper {
    public:
        /**
         * Wrap an entity according to its type. Must have the corresponding tag !
         */
        explicit EntityWrapper(flecs::entity entity) : m_entity{entity}{};

        /**
         * @return The underlying entity. For more information see : https://flecs.docsforge.com/master/manual/#entity.
         */
        inline flecs::entity entity() { return m_entity; }
        inline operator flecs::entity() { return m_entity; }

    private:
        flecs::entity m_entity;
    };

    class Action : public EntityWrapper{
    public:
        explicit Action(flecs::entity entity) : EntityWrapper(entity){};
    };

    // Forward declaration
    class Reasonner;

    template<typename T>
    struct Process{};

    class Agent : public EntityWrapper{
    public:
        explicit Agent(flecs::entity entity) : EntityWrapper(entity) {};

        template<typename T>
        void reason() { 
			static_assert(std::is_base_of<Reasonner, T>::value, "Wrong type passed, must be a Reasonner.");
            entity().add<Process<T>>();
        };

        const char* name() { return entity().name(); }
    };

    class Artefact : public EntityWrapper{
    public:
        explicit Artefact(flecs::entity entity) : EntityWrapper(entity){};
    };
    class Organisation : public EntityWrapper{
    public:
        explicit Organisation(flecs::entity entity) : EntityWrapper(entity){};
    };

    /**
     * Manipulate a percept
     */
    class Percept : public EntityWrapper{
    public:
        /**
         * Instantiate a handler for manipulating percept.
         * @param entity must be a percept !
         */
        explicit Percept(flecs::entity entity) : EntityWrapper(entity){};

        /**
         * Add a relation "relation::perceive" from the given entity e to this percept
         * @param e entity perceiving this percept
         * @return
         */
        Percept& perceived_by(flecs::entity e){
            e.mut(e).add<relation::perceive>(entity());
            return *this;
        }

        /**
         * Add a relation "relation::perceive" from the given entity e to this percept
         * @param e entity perceiving this percept
         * @return
         */
        Percept& perceived_by(flecs::entity_view e){
            e.mut(entity()).add<relation::perceive>(entity());
            return *this;
        }

        /**
         * This percept will be destroyed after the specified amount
         * @param ttl how much time should this entity live ?
         * @return
         */
        Percept& decay(float ttl = 2.0f){
            entity().set<component::Decay>({ttl});
            return *this;
        }
    };

    /**
     * Builder to create an entity with specified type.
     * @tparam TObj Object returned by the builder
     * @tparam TTag Tad added to the entity
     */
    template<typename TObj, typename TTag>
    class Builder {
    protected:
        flecs::world& world;
        flecs::entity entity;

    public:
        explicit Builder(flecs::world& world) : world{world}, entity{world.entity()}{
            entity.add<TTag>();
        };

        explicit Builder(flecs::world& world, const char * name) : world{world}, entity{world.entity(name)}{
            entity.add<TTag>();
        };
    };

    class ActionBuilder : public Builder<Action, type::Action> {
    public:
        explicit ActionBuilder(flecs::world& world, const char * name) : Builder<Action, type::Action>(world, name){};

        Action build() {
            return Action(entity);
        }
    };

    class AgentBuilder : public Builder<Agent, type::Agent> {
    public:
        explicit AgentBuilder(flecs::world& world, const char * name) : 
            Builder<Agent, type::Agent>(world, name)
        {};

        Agent build() {
            return Agent(entity);
        }
    };

    class ArtefactBuilder : public Builder<Artefact, type::Artefact> {
    public:
        explicit ArtefactBuilder(flecs::world& world, const char * name) : Builder<Artefact, type::Artefact>(world, name){};

        Artefact build() {
            return Artefact(entity);
        }
    };

    class OrganisationBuilder : public Builder<Organisation, type::Organisation> {
    public:
        explicit OrganisationBuilder(flecs::world& world, const char * name) : Builder<Organisation, type::Organisation>(world, name){};

        Organisation build() {
            return Organisation(entity);
        }
    };

    /**
     * Specialization for building percepts as we need to specify its type at creation and its source.
     */
    class PerceptBuilder : public Builder<Percept, type::Percept> {
    public:
        /**
         * Builder to instantiate a new percept within the specified world
         * @param world
         */
        explicit PerceptBuilder(flecs::world& world) : Builder<Percept, type::Percept>(world){};

        /**
         * A percept must be coming from a source, to add a relation "percept source object"
         * @tparam Sense type of sense
         * @param source entity responsible for the creation of this percept
         * @return
         */
        template<typename Sense>
        Percept source(flecs::entity source) {
            return Percept(entity
                    .add<relation::source>(source)
                    .add<relation::perceive>(source)
                    .add<Sense>()
            );
        }
    };
}// namespace dynamo
#endif //DYNAMO_TYPES_HPP
