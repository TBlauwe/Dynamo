//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <flecs.h>

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

    template<typename T>
    class Perception{
    public:
        struct Percept{
            float ttl;
        };

        explicit Perception(flecs::world& world) : world{world}{
            world.system<Perception<T>::Percept>()
                .each([](flecs::entity e, Perception<T>::Percept& percept) {
                    percept.ttl -= e.delta_time();
                });
        }

        void add_percept(T percept){
            world.entity().add<Perception<T>::Percept>(percept);
        };

    private:
        flecs::world& world;
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
