#ifndef DYNAMO_PERCEPTION_HPP
#define DYNAMO_PERCEPTION_HPP

#include <dynamo/modules/core.hpp>

namespace dynamo{
    namespace component{
        struct DecayingPercept{
            float ttl;
        };

        struct Integer{
            int value;
        };

        struct RandomIntEmitter{
            int value_min {0};
            int value_max {10};
            float delay_min {1.0f};
            float delay_max {2.0f};
            float last_emission {0.f};
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
            explicit Perception(flecs::world& world) : world{world}{
                // ========== Phase ==========
                world.system<component::DecayingPercept>()
                        .kind(flecs::OnLoad)
                        .each([](flecs::entity e, component::DecayingPercept& percept) {
                            if(percept.ttl <= 0.f)
                                e.destruct();
                            else
                                percept.ttl -= e.delta_time();
                        });

                world.system<component::RandomIntEmitter>()
                        .kind(flecs::OnUpdate)
                        .each([](flecs::entity e, component::RandomIntEmitter& emitter) {
                            if(emitter.last_emission <= 0.f){
                                emitter.last_emission = emitter.delay_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(emitter.delay_max-emitter.delay_min)));
                                auto percept = e.world().entity();
                                percept.add<tag::Percept>();
                                percept.set<component::DecayingPercept>({2.0f});
                                percept.set<component::Integer>({emitter.value_min + rand() % emitter.value_max});
                                e.world().set<component::Event>({fmt::format("{} is emitting", e.name())});
                            }
                            else
                                emitter.last_emission -= e.delta_time();
                        });

                world.system<tag::Percept>("OnAddPercept")
                        .kind(flecs::OnAdd)
                        .each([&world](flecs::entity entity_percept, const tag::Percept&){
                            world.each([&entity_percept](flecs::entity entity_agent, const tag::Agent&){
                                entity_agent.add<relation::sees>(entity_percept);
                            });
                        });
            }

            template<typename TPerceptType, typename TData>
            flecs::entity add_percept(TPerceptType percept_type, TData data){
                auto entity_percept = world.entity();
                entity_percept.add<dynamo::tag::Percept>();
                entity_percept.set<TPerceptType>(percept_type);
                entity_percept.set<TData>(data);
                return entity_percept;
            }
        };
    }
}

#endif //DYNAMO_PERCEPTION_HPP
