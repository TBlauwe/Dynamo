//
// Created by Tristan on 09/09/2021.
//
#include <dynamo/dynamo.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>

dynamo::Simulation::Simulation() :
        logger(spdlog::stdout_color_mt("Dynamo")),
        world()
{
    // Logger initialization
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%n] %^(%8l)%$ %v");

    logger->info("Initialization complete !");
}

void dynamo::Simulation::run(){
    logger->info("running ...");
}

void dynamo::Simulation::shutdown(){
    logger->info("Shutdown complete !");
}