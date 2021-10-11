#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <flecs.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace dynamo{

    enum class Level{
        Minor,
        Medium,
        Major
    };

    namespace type{
        struct Action{};
        struct Agent{};
        struct Artefact{};
        struct Event{
            Level   level{Level::Minor};
        };
        struct Organisation{};
        struct Percept{};
    }

    namespace tag{
        struct CurrentFrame{};
    }

    namespace relation{
        template<typename TType>
        struct perceive{};
        struct source{};
        struct belongs{};
    }
    namespace component{
        template<typename T>
        struct InitialValue{
            T memory;
        };

        struct Decay{
            float ttl;
        };

        struct Cooldown{
            float remaining_time;
        };

        struct Tick{
            int32_t tick;
        };

        struct Targets{
            std::vector<flecs::entity_view> entities;
        };
    }

    namespace module{
        struct Core{
            explicit Core(flecs::world& world);

            // ===== PREFAB =====
            flecs::entity Action;
            flecs::entity Agent;
            flecs::entity Artefact;
            flecs::entity Event;
            flecs::entity Organisation;
            flecs::entity Percept;

            // ===== QUERY =====
            flecs::query<type::Agent> agents_query;

            // ===== INTERNAL =====
            flecs::entity Feature;
        };
    }

    template<typename TTag, typename TTerm>
    void add_tag_to(flecs::world& world, const char * tag_name, const char * term_name){
        world.system<const TTerm>(fmt::format("OnAdd_{}_Add{}", term_name, tag_name).c_str())
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const TTerm& term){
                    e.set<TTag>({});
                });
    }

    flecs::type create_feature(flecs::world& world, const char *name, const char * description = "");

    template<typename T>
    T& module_ref(flecs::entity e) {
        return *e.get_mut<T>();
    }

    template<typename T>
    const T& module_cref(flecs::entity e) {
        return *e.get<T>();

    }
}

#endif //DYNAMO_CORE_H
