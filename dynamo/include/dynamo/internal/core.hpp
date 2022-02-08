#pragma once

#include <vector>

#include <effolkronium/random.hpp>

#include <dynamo/internal/types.hpp>

/**
@file dynamo/internal/core.hpp
@brief Define a mandatory module to setup core functionnalities
*/


/**
@brief Namespace containing modules
*/
namespace dynamo
{
    using Random    = effolkronium::random_static;
    using RandomTS  = effolkronium::random_thread_local;

    namespace module {
        /**
        @brief Core module to setup core functionnalities
        */
        struct Core {
            /**
            @brief Core is a flecs::module that requires a world to instantiate.
            */
            explicit Core(flecs::world& world);
        };
    }
}
