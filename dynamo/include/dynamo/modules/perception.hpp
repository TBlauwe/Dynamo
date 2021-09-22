#ifndef DYNAMO_PERCEPTION_HPP
#define DYNAMO_PERCEPTION_HPP

#include <dynamo/modules/core.hpp>

namespace dynamo{
    namespace component{
        struct DecayingPercept{
            float ttl;
        };
    }

    namespace tag{
        struct Percept{};
    }

    namespace relation{
        struct sees{};
    }

    namespace module{
        class Perception{
        private:
            flecs::world& world;

        public:
            explicit Perception(flecs::world& world) :
                    world{world}
            {
                // ========== Phase ==========
                // 1. --- OnLoad
                world.system<component::DecayingPercept>()
                        .kind(flecs::OnLoad)
                        .each([](flecs::entity e, component::DecayingPercept& percept) {
                            if(percept.ttl <= 0.f)
                                e.destruct();
                            else
                                percept.ttl -= e.delta_time();
                        });

                world.system<tag::Percept>("OnSetPercept")
                        .kind(flecs::OnSet)
                        .each([&world](flecs::entity entity_percept, const tag::Percept){
                            world.each([&entity_percept](flecs::entity entity_agent, const tag::Agent){
                                entity_agent.add<relation::sees>(entity_percept);
                            });
                        });
            }
        };
    }
}

#endif //DYNAMO_PERCEPTION_HPP
