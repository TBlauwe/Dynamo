//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <flecs.h>

namespace dynamo{
    class Simulation{
    public:
        Simulation();

        void run();
        void shutdown();

        flecs::world world;

    private:
        std::shared_ptr<spdlog::logger> logger;
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
