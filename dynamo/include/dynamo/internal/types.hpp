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
     * Common function used by type handler.
     * @tparam T
     */
    template<typename T>
    class TypeHandler {
        flecs::entity _entity;
    public:
        /**
         * Wrap an entity according to its type. Must have the corresponding tag !
         * @tparam T Type (Agent, Organisation, etc.)
         */
        explicit TypeHandler(flecs::entity entity) : _entity{entity}{
            assert(entity.has<T>());
        };

        /**
         * @return The underlying entity. For more information see : https://flecs.docsforge.com/master/manual/#entity.
         */
        flecs::entity entity() { return _entity; }
    };

    /**
     * Builder to create an entity with specified type.
     * @tparam T Type of the entity.
     */
    template<typename T>
    class TypeBuilder {
        flecs::world& world;
    public:
        /**
         * Create a builder for the specified type.
         * @param world
         */
        explicit TypeBuilder(flecs::world& world) : world{world}{};

        /**
         * return an entity with the specified name and the correspoding component.
         * @param name Name of the entity
         * @return A handler to manipulate the entity
         */
        TypeHandler<T> create(const char * name) { return TypeHandler<T>(world.entity(name).add<T>()); }
    };

    /**
     * Handler to manipulate a percept.
     */
    template<>
    class TypeHandler<type::Percept>{
        flecs::entity _entity;
    public:
        /**
         * Instantiate a handler for manipulating percept.
         * @param entity must be a percept !
         */
        explicit TypeHandler(flecs::entity entity) : _entity{entity}{
            assert(entity.has<type::Percept>());
        };

        /**
         * @return The underlying entity. For more information see : https://flecs.docsforge.com/master/manual/#entity.
         */
        flecs::entity entity() { return _entity; }

        /**
         * Add a relation "relation::perceive" from the given entity e to this percept
         * @param e entity perceiving this percept
         * @return
         */
        TypeHandler<type::Percept>& perceived_by(flecs::entity e){
            e.mut(e).add<relation::perceive>(_entity);
            return *this;
        }

        /**
         * Add a relation "relation::perceive" from the given entity e to this percept
         * @param e entity perceiving this percept
         * @return
         */
        TypeHandler<type::Percept>& perceived_by(flecs::entity_view e){
            e.mut(_entity).add<relation::perceive>(_entity);
            return *this;
        }

        /**
         * This percept will be destroyed after the specified amount
         * @param ttl how much time should this entity live ?
         * @return
         */
        TypeHandler<type::Percept>& decay(float ttl = 2.0f){
            _entity.set<component::Decay>({ttl});
            return *this;
        }
    };

    /**
     * Specialization for building percepts as we need to specify its type at creation and its source.
     */
    template<>
    class TypeBuilder<type::Percept> {
        flecs::world& world;
    public:
        /**
         * Builder to instantiate a new percept within the specified world
         * @param world
         */
        explicit TypeBuilder(flecs::world& world) : world{world}{};

        /**
         * A percept must be coming from a source, to add a relation "percept source object"
         * @tparam T
         * @param source entity responsible for the creation of this percept
         * @return
         */
        template<typename T>
        TypeHandler<type::Percept> source(flecs::entity source) {
            return TypeHandler<type::Percept>(world.entity()
                .add<type::Percept>()
                .add<relation::source>(source)
                .add<T>());
        }
    };

}// namespace dynamo
#endif //DYNAMO_TYPES_HPP
