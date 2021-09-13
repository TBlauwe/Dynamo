//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <flecs.h>

class Dynamo{
public:
    Dynamo();

    void run();
    void shutdown();

private:
    std::shared_ptr<spdlog::logger> logger;
};

#endif //DYNAMO_DYNAMO_HPP
