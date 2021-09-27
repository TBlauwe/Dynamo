//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <dynamo/modules/core.hpp>
#include <dynamo/modules/perception.hpp>

namespace dynamo{
    class Simulation{
    public:
        flecs::world world {};

        flecs::query<const tag::Agent>      agents_query    {world.query<const tag::Agent>()};
        flecs::query<const tag::Artefact>   artefacts_query {world.query<const tag::Artefact>()};

    public:
        Simulation();

        flecs::entity   add_agent(const char * name) const;
        flecs::entity   add_artefact(const char * name) const;
        void            add_event(const char * name) const;

        void run() const;
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
