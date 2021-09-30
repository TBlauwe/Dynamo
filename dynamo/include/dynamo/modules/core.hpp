#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <flecs.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace dynamo{

    namespace singleton{
        struct Logger{
            std::shared_ptr<spdlog::logger> logger {spdlog::stdout_color_mt("Dynamo")};
            Logger();
        };
    }
    std::shared_ptr<spdlog::logger> logger(flecs::entity e);
    std::shared_ptr<spdlog::logger> logger(flecs::id id);
    std::shared_ptr<spdlog::logger> logger(flecs::world& world);

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
            T   memory;
        };

        struct Decay{
            float ttl;
        };

        struct Tick{
            int32_t tick;
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
        };
    }

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
