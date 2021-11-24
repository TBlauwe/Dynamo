#ifndef DYNAMO_CORE_HPP
#define DYNAMO_CORE_HPP

#include <vector>

#include <dynamo/internal/types.hpp>

/**
@file dynamo/internal/core.hpp
@brief Define a mandatory module to setup core functionnalities
*/


/**
@brief Namespace containing modules
*/
namespace dynamo::module {
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

#endif //DYNAMO_CORE_HPP
