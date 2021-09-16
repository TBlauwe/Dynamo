//
// Created by Tristan on 09/09/2021.
//
#ifndef DYNAMO_DYNAMO_HPP
#define DYNAMO_DYNAMO_HPP

#include <spdlog/spdlog.h>
#include <flecs.h>

namespace dynamo{
    class Simulation{
    public:
        Simulation();

        void run();
        void shutdown();

        flecs::world world;
    };

    template<typename T>
    struct Perception{
        explicit Perception(flecs::world& world, float interval = 1.0){
            world.system<Perception<T>::Buffer>()
                .interval(interval)
                .each([](flecs::entity e, const Perception<T>::Buffer& buffer) {
                });

            world.system<Perception<T>::Buffer>()
                    .interval(1.0)
                    .kind(flecs::PostUpdate)
                    .each([](flecs::entity e, Perception<T>::Buffer& buffer) {
                        buffer.data.clear();
                    });
        }

        struct Percept{
            T data;
        };

        struct Buffer{
            std::vector<T> data;
        };

        void add_perception(flecs::entity entity){
            entity.add<Perception<T>::Buffer>();
        };

        void add_percept(flecs::entity entity, T percept){
            Perception<T>::Buffer* buffer = entity.template get_mut<Perception<T>::Buffer>();
            buffer->data.emplace_back(percept);
            entity.template modified<Perception<T>::Buffer>();
        };
    };
}//namespace dynamo

#endif //DYNAMO_DYNAMO_HPP
