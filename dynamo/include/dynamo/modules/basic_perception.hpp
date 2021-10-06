#ifndef DYNAMO_BASIC_PERCEPTION_HPP
#define DYNAMO_BASIC_PERCEPTION_HPP

#include <dynamo/modules/core.hpp>

namespace dynamo{
    namespace senses{
        struct Hearing{};
        struct Smell{};
        struct Taste{};
        struct Touch{};
        struct Vision{};
        struct Global{};
    };

    namespace component{
        struct PeriodicEmitter{
            float cooldown;
        };
    }

    namespace module{
        struct GlobalPerception{
            explicit GlobalPerception(flecs::world& world){
                world.module<GlobalPerception>();
                auto& core = dynamo::module_ref<module::Core>(world.import<module::Core>());
                info_module_header(logger(world), "Perception");

                world.system<component::PeriodicEmitter, component::Targets>("PeriodicEmitter")
                        .term<component::Cooldown>().object<component::PeriodicEmitter>().oper(flecs::Not)
                        .each([&core](flecs::entity e, component::PeriodicEmitter& periodicEmitter, component::Targets& targets){
                            flecs::entity percept = e.world().entity()
                                    .is_a(core.Percept)
                                    .set<dynamo::component::Decay>({2.0f})
                                    .add<dynamo::relation::source>(e);
                            for(flecs::entity_view& entity_view : targets.entities){
                                entity_view.mut(e).add<relation::perceive<senses::Hearing>>(percept);
                            }
                            e.set<component::Cooldown, component::PeriodicEmitter>({periodicEmitter.cooldown});
                        });
            }
        };
    }
}

#endif //DYNAMO_BASIC_PERCEPTION_HPP