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

    namespace module{
        class Perception{
        private:
            flecs::world& world;
            flecs::query<const tag::Agent> agents_query;

        public:
            // ===== Relations =====
            flecs::entity sees;

        public:
            explicit Perception(flecs::world& world) :
                    world{world},
                    sees{world.entity()},
                    agents_query{world.query<const tag::Agent>()}
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


            }

            template<class TPerceptType, class TData>
            void add(TPerceptType percept_type, TData data){
                auto entity_percept = world.entity();
                entity_percept.add<tag::Percept>();
                entity_percept.set<TPerceptType>(percept_type);
                entity_percept.set<TData>(data);
                //agents_query.each([&](flecs::entity entity_agent, const tag::Agent agent) {
                    //entity_agent.add(sees, entity_percept);
                //});
            };
        };
    }
}

#endif //DYNAMO_PERCEPTION_HPP
