//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <dynamo/modules/perception.hpp>
#include <flecs.h>

namespace dynamo{
    class Simulation{
    public:
        std::shared_ptr<spdlog::logger> logger;

        flecs::world world;
        module::Perception perception;

        flecs::query<const tag::Agent> agents_query;

    public:
        Simulation();

        flecs::entity add_agent(const char * name) const{
            auto e = world.entity(name);
            e.add<tag::Agent>();
            logger->info("Added agent : {} - {}", name, e);
            return e;
        }

        void add_event(const char * name) const{
            logger->info("Added event : {}", name);
            world.set<component::Event>({name});
        }

        void run() const;
        void shutdown() const;

    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
