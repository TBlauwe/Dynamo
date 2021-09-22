//
// Created by Tristan on 09/09/2021.
//
#include <dynamo/dynamo.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

dynamo::Simulation::Simulation() :
        logger(spdlog::stdout_color_mt("Dynamo")),
        world{},
        perception{world},
        agents_query{world.query_builder<const tag::Agent>().build()}
{
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%10n] %^(%8l)%$ %v");
    logger->info("Dynamo is launching ...");
    // Do some stuff here
    logger->info("Dynamo launched !");
}

void dynamo::Simulation::run() const{
    world.progress();
}

void dynamo::Simulation::shutdown() const{
    logger->info("Dynamo is shutting down ...");
    // Do some stuff here
    logger->info("Dynamo shutdown !");
}