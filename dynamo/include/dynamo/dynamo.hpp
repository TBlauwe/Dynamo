//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <dynamo/modules/perception.hpp>
#include <flecs.h>

namespace dynamo{
    class Simulation{
    public:
        std::shared_ptr<spdlog::logger> logger {spdlog::stdout_color_mt("Dynamo")};

        flecs::world world {};
        module::Perception perception {world};

        flecs::query<const tag::Agent>      agents_query    {world.query<const tag::Agent>()};
        flecs::query<const tag::Artefact>   artefacts_query {world.query<const tag::Artefact>()};

    public:
        Simulation();

        flecs::entity   add_agent(const char * name) const;
        flecs::entity   add_artefact(const char * name) const;
        void            add_event(const char * name) const;

        void run() const;
        void shutdown() const;
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
