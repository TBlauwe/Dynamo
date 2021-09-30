#ifndef DYNAMO_BASIC_PERCEPTION_HPP
#define DYNAMO_BASIC_PERCEPTION_HPP

#include <dynamo/modules/core.hpp>

namespace dynamo{
    enum class Senses{
        HEARING,
        SMELL,
        TASTE,
        TOUCH,
        VISION,
        GLOBAL
    };

    namespace component{
        struct Cooldown{
            float   value;
        };
    }

    namespace module{
        struct GlobalPerception{
            explicit GlobalPerception(flecs::world& world){
                world.module<GlobalPerception>();
                auto& core = dynamo::module_ref<module::Core>(world.import<module::Core>());
                logger(world)->info("Loading module : GlobalPerception");

                world.system<const component::Cooldown>("OnSet_Cooldown_RememberInitialValue")
                        .kind(flecs::OnSet)
                        .each([](flecs::entity e, const component::Cooldown& cooldown) {
                            e.set<component::InitialValue<component::Cooldown>>({cooldown.value});
                        });

                world.system<component::Cooldown, const component::InitialValue<component::Cooldown>>("Cooldown_Perception")
                        .kind(flecs::PreFrame)
                        .each([&core](flecs::entity e, component::Cooldown& cooldown, const component::InitialValue<component::Cooldown>& initial_value) {
                            if(cooldown.value <= 0.f){
                                cooldown.value = initial_value.memory.value;
                                e.world().entity()
                                        .is_a(core.Percept)
                                        .set<component::Decay>({2.0f})
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