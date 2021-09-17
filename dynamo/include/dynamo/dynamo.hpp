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
        }

        template<class TPerceptType, class TData>
        void add_percept(TPerceptType percept, TData data){
            std::cout << "Time to live \n";
            auto e = world.entity();
            e.set<percept>(percept);
            e.set<data>(data);
        };

    private:
        flecs::world& world;
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
