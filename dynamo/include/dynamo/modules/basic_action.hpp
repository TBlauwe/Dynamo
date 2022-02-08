#pragma once

#include <flecs.h>

namespace dynamo{

	struct Cost{
		int value {0};
	};

    namespace module{
        /**
         * Module adding perception functionalities.
         */
        struct BasicAction{
            /**
             * Module adding perception functionalities.
             */
            explicit BasicAction(flecs::world& world){
                world.module<BasicAction>();
            }
        };
    }
}
