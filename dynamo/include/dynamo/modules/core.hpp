#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <flecs.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace dynamo{

    /**
     *  Namespace used to collect tag/components that are mutually exclusive (An agent cannot be an organisation, etc.).
     */
    namespace type{
        /**
         * Tag used to mark an entity as an action.
         */
        struct Action{};

        /**
         * Tag used to mark an entity as an agent.
         */
        struct Agent{};

        /**
         * Tag used to mark an entity as an artefact.
         */
        struct Artefact{};

        /**
         * Tag used to mark an entity as an organisation.
         */
        struct Organisation{};

        /**
         * Tag used to mark an entity as a percept .
         */
        struct Percept{};
    }

    namespace tag{
        /**
         * Tag removed at the end of the current frame. Beware to handle this tag before flecs::PostFrame /!\ .
         */
        struct CurrentFrame{};
    }

    namespace relation{
        /**
         * Relation from an entity A to a percept B : "A perceive<TType> B"
         * @tparam TType By which means A perceive B, typically a sense like hearing, etc.
         */
        template<typename TType>
        struct perceive{};

        /**
         * Relation from an entity A to an entity B : "A source B" => "A is coming from B".
         * Used notably to store the source of a percept.
         */
        struct source{};

        /**
         * Relation from an entity A to an organisation B : "A belongs B" => "A belongs to B".
         */
        struct belongs{};
    }
    namespace component{
        template<typename T>
        struct InitialValue{
            T memory;
        };

        /**
         * When ttl reaches 0, the entity holding this component is destroyed.
         */
        struct Decay{
            float ttl;
        };

        /**
         * Must be used as a relation with something else.
         * When remaining_time reaches 0, the relation for this cooldown is removed.
         */
        struct Cooldown{
            float remaining_time;
        };

        /**
         * Store read-only entities handle. Call mut(...), if you want to modify it.
         */
        struct Targets{
            std::vector<flecs::entity_view> entities;
        };
    }

    namespace module{
        /**
         * Defines basics concepts and entities used throughout the simulation.
         */
        struct Core{
            explicit Core(flecs::world& world);

            // ===== PREFAB =====
            flecs::entity Action;
            flecs::entity Agent;
            flecs::entity Artefact;
            flecs::entity Organisation;
            flecs::entity Percept;

            // ===== QUERY =====
            /**
             * Use this query to quickly iterate over all agents.
             */
            flecs::query<type::Agent> agents_query;
        };
    }

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

    /**
     * Helper function to retrieve a module ref from an entity
     * TODO Useless - delete
     * @tparam T
     * @param e
     * @return
     */
    template<typename T>
    T& module_ref(flecs::entity e) {
        return *e.get_mut<T>();
    }
}

#endif //DYNAMO_CORE_H
