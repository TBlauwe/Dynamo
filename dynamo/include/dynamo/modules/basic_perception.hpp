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
        struct Cooldown{
            float value;
        };
    }

    namespace module{
        struct GlobalPerception{
            explicit GlobalPerception(flecs::world& world){
                world.module<GlobalPerception>();
                auto& core = dynamo::module_ref<module::Core>(world.import<module::Core>());
                info_module_header(logger(world), "Perception");

                world.system<const component::Cooldown>("OnSet_Cooldown_RememberInitialValue")
                        .kind(flecs::OnSet)
                        .each([](flecs::entity e, const component::Cooldown& cooldown) {
                            e.set<component::InitialValue<component::Cooldown>>({{cooldown.value}});
                        });

                world.system<component::Cooldown, const component::InitialValue<component::Cooldown>>()
                        .kind(flecs::PreFrame)
                        .each([&core](flecs::entity e, component::Cooldown& cooldown, const component::InitialValue<component::Cooldown>& initial_value) {
                            if(cooldown.value <= 0.f){
                                cooldown.value = initial_value.memory.value;
                                //auto core = e.world().get<dynamo::module::Core>();

                                e.world().entity()
                                        .is_a(core.Percept)
                                        .add<dynamo::senses::Hearing>()
                                        .set<dynamo::component::Decay>({2.0f})
                                        .add<dynamo::relation::source>(e);
                            }
                            else{
                                cooldown.value -= e.delta_time();
                            }
                        });
            }
        };
    }
}

#endif //DYNAMO_BASIC_PERCEPTION_HPP