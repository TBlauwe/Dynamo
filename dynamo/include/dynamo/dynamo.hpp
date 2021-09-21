//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <flecs.h>
#include <iostream>

namespace dynamo{

    struct Agent{};

    class Simulation{
    public:
        Simulation();

        flecs::entity add_agent(const char * name){
            auto e = world.entity(name);
            e.add<Agent>();
            return e;
        }

        void run();
        void shutdown();

        flecs::world world;
    };

    struct DecayingPercept{
        float ttl;
    };

    class Perception{
    public:

        explicit Perception(flecs::world& world) : world{world}{
            world.system<DecayingPercept>()
                .kind(flecs::PreUpdate)
                .each([](flecs::entity e, DecayingPercept& percept) {
                    if(percept.ttl <= 0.f)
                        e.destruct();
                    else
                        percept.ttl -= e.delta_time();
                });

            world.system<Agent>()
                    .interval(1)
                    .each([&](flecs::entity e, const Agent& agent) {
                        world.each([&](const int data){
                            std::cout << e.name() << " is perceiving " << data << std::endl;
                        });
                    });
        }

        template<class TPerceptType, class TData>
        void add_percept(TPerceptType percept, TData data){
            auto pe = world.entity();
            pe.set<percept>(percept);
            pe.set<data>(data);
            world.each([&](flecs::entity e, const Agent& agent) {
                e.add(is_seen_by, pe);
                    });
        };

    private:
        flecs::world& world;
        flecs::entity is_seen_by;
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
