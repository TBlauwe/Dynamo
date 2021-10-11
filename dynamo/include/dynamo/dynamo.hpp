#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <dynamo/modules/core.hpp>
#include <dynamo/modules/basic_perception.hpp>

namespace dynamo{
    class Simulation{
        flecs::world world {};

    public:
        Simulation();

        void step();
        void step_n(unsigned int n = 0);
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
