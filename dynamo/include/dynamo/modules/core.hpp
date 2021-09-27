#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <flecs.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace dynamo{
    namespace tag{
        struct Agent{};
        struct Artefact{};
        struct Action{};
        struct Organisation{};
    }

    namespace component{
        template<typename T>
        struct Event{
            std::string name;
        };
    }

    namespace event{
        struct MINOR{};
        struct MAJOR{};
        struct CUSTOM_A{};
        struct CUSTOM_B{};
        struct CUSTOM_C{};
        struct CUSTOM_D{};
    }

    namespace relation{
        struct is_in{};
    }

    namespace singleton{
        struct Logger{
            std::shared_ptr<spdlog::logger> logger {spdlog::stdout_color_mt("Dynamo")};

            Logger(){
                logger->set_level(spdlog::level::trace);
                logger->set_pattern("[%10n] %^(%8l)%$ %v");
                logger->info("Dynamo launching ...");
            };
        };
    }

    namespace module{
        struct Core{
            explicit Core(flecs::world& world){
                world.module<Core>();
                world.add<singleton::Logger>();
            }
        };
    }
}

#endif //DYNAMO_CORE_H
