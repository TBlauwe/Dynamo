#pragma once

#include <dynamo/internal/core.hpp>
#include <string>

namespace dynamo {

	/**
	@brief Struct holding a value correspoding to the current stress
	*/
	struct Stress {
		/**
		@brief Stress' value oscillating between 0 and 100.
		*/
		float value { 100};
	};

    inline bool is_pressured(const flecs::entity& e)
    {
        return e.has<Stress>() && e.get<Stress>()->value > 10;
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

                world.system<Stress>()
                    .kind(flecs::PreUpdate)
                    .each([](flecs::entity entity, Stress& stress)
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