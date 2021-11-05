#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <dynamo/internal/types.hpp>
#include <vector>

/**
@file dynamo/internal/core.hpp
@brief Define a mandatory module to setup core functionnalities
*/
namespace dynamo::module{
        /**
        @brief Core module to setup core functionnalities
        */
        struct Core{
            /**
            @brief Core is a flecs::module that requires a world to instantiate.
            */
            explicit Core(flecs::world& world);
        };
    }

#endif //DYNAMO_CORE_H
