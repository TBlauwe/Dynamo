//
// Created by Tristan on 09/09/2021.
//
#include <dynamo/dynamo.hpp>

#include <doctest/doctest.h>
#include <spdlog/sinks/stdout_color_sinks.h>

Dynamo::Dynamo() :
        logger(spdlog::stdout_color_mt("Dynamo")){
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%n] %^(%8l)%$ %v");
    logger->info("Initialization complete !");
}

void Dynamo::run(){
    logger->info("running ...");
}

void Dynamo::shutdown(){
    logger->info("Shutdown complete !");
}