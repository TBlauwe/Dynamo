#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <dynamo/modules/core.hpp>
#include <dynamo/modules/basic_perception.hpp>

namespace dynamo{
    struct Simulation{
        explicit Simulation(flecs::world& world);
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
