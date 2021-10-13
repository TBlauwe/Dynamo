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
     * Common function used by type handler
     * @tparam T
     */
    template<typename T>
    class TypeHandler {
        flecs::entity _entity;
    public:
        explicit TypeHandler(flecs::entity entity) : _entity{entity}{};
        flecs::entity entity() { return _entity; }
    };

    /**
     * Common function used by type builder
     * @tparam T
     */
    template<typename T>
    class TypeBuilder {
        flecs::world& world;
    public:
        explicit TypeBuilder(flecs::world& world) : world{world}{};
        TypeHandler<T> create(const char * name) { return TypeHandler<T>(world.entity(name).add<T>()); }
    };

    template<>
    class TypeHandler<type::Percept>{
        flecs::entity _entity;
    public:
        explicit TypeHandler(flecs::entity entity) : _entity{entity}{};
        flecs::entity entity() { return _entity; }

        TypeHandler<type::Percept>& perceived_by(flecs::entity e){
            e.mut(e).add<relation::perceive>(_entity);
            return *this;
        }

        TypeHandler<type::Percept>& perceived_by(flecs::entity_view e){
            e.mut(_entity).add<relation::perceive>(_entity);
            return *this;
        }

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
        explicit TypeBuilder(flecs::world& world) : world{world}{};

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
