#include <dynamo/internal/core.hpp>

namespace dynamo{
    module::Core::Core(flecs::world &world) {
        world.module<Core>();

        // ========== Pipeline ==========

        auto decay_system = world.system<component::Decay>("Decay")
                .kind(flecs::PreFrame)
                .each([](flecs::entity e, component::Decay& decay) {
                    if(decay.ttl <= 0.f){
                        e.destruct();
                    }
                    else{
                        decay.ttl -= e.delta_time();
                    }
                });

        world.type("Feature_Decay")
            .add(decay_system);

        world.system<component::Cooldown>("Cooldown")
                .arg(1).object(flecs::Wildcard) // <- Cooldown is actually a pair type with anything
                .kind(flecs::PreFrame)
                .iter([](flecs::iter& iter, component::Cooldown* cooldown) {
                    for(auto i : iter){
                        cooldown[i].remaining_time -= iter.delta_time();
                        if(cooldown[i].remaining_time <= 0){
                            iter.entity(i).remove<component::Cooldown>(iter.term_id(1).object());
                        }
                    }
                });

        world.system<tag::CurrentFrame>("RemoveCurrentFrameTag")
                .kind(flecs::PostFrame)
                .each([](flecs::entity e, tag::CurrentFrame) {
                    e.remove<tag::CurrentFrame>();
                });
    }
}
