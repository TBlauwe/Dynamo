//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_PERSONA_HPP
#define DYNAMO_PERSONA_HPP

#include <spdlog/spdlog.h>
#include <flecs.h>

class Dynamo{
public:
    Dynamo();

    std::shared_ptr<spdlog::logger> logger;

    void run();
    void shutdown();
};

#endif //DYNAMO_PERSONA_HPP
