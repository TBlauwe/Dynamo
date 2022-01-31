#ifndef DYNAMO_BASIC_ACTION_HPP
#define DYNAMO_BASIC_ACTION_HPP

#include <flecs.h>

namespace dynamo{

    namespace type{
        struct Cost{
            int value {0};
        };
    }

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

#endif //DYNAMO_BASIC_PERCEPTION_HPP
