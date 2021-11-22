#ifndef DYNAMO_MODULES_BASIC_STRESS_HPP
#define DYNAMO_MODULES_BASIC_STRESS_HPP

#include <dynamo/internal/core.hpp>
#include <string>

namespace dynamo {

    namespace type {
        /**
        @brief Struct holding a value correspoding to the current stress
        */
        struct Stress {
            /**
            @brief Stress' value oscillating between 0 and 100.
            */
            float value { 100};
        };
    }

    namespace module {
        /**
        @brief Example module for adding stress to agent with passive decreasing.
        */
        struct BasicStress {
            /**
            @brief Module responsible for managing basic stress functionnality.
            */
            explicit BasicStress(flecs::world& world) {
                world.module<BasicStress>();
                world.import<module::Core>();

                world.system<type::Stress>()
                    .kind(flecs::PreUpdate)
                    .each([](flecs::entity entity, type::Stress& stress)
                        {
                            if (stress.value > 0.f)
                            {
                                stress.value -= entity.delta_time();
                            }
                        }
                );
            }
        };
    }
}
#endif//DYNAMO_MODULES_BASIC_STRESS_HPP