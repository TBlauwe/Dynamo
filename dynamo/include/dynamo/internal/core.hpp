#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <dynamo/internal/types.hpp>
#include <vector>

namespace dynamo::module{
        /**
         * Load core systems
         */
        struct Core{
            /**
             * Core is a flecs::module that requires a world to instantiate.
             * @param world
             */
            explicit Core(flecs::world& world);
        };
    }

#endif //DYNAMO_CORE_H
