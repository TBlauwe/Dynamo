#include <dynamo/internal/core.hpp>

namespace dynamo {
    module::Core::Core(flecs::world& world) {
        world.module<Core>();

        // =========================================================================== 
        // Components
        // =========================================================================== 

        auto e = world.component<type::perceive>();
        e.add(flecs::Transitive);

        // =========================================================================== 
        // Pipeline
        // =========================================================================== 

        auto decay_system = world.system<type::Decay>("Decay")
            .kind(flecs::PreFrame)
            .each([](flecs::entity e, type::Decay& decay) {
            if (decay.ttl <= 0.f) {
                e.destruct();
            }
            else {
                decay.ttl -= e.delta_time();
            }
                });

        world.system<type::Cooldown>("Cooldown")
            .arg(1).obj(flecs::Wildcard) // <- Cooldown is actually a pair type with anything
            .kind(flecs::PreFrame)
            .iter([](flecs::iter& iter, type::Cooldown* cooldown) {
            for (auto i : iter) {
                cooldown[i].remaining_time -= iter.delta_time();
                if (cooldown[i].remaining_time <= 0) {
                    iter.entity(i).remove<type::Cooldown>(iter.id(1).object());
                }
            }
                });

        world.system<type::CurrentFrame>("RemoveCurrentFrameTag")
            .kind(flecs::PostFrame)
            .each([](flecs::entity e, type::CurrentFrame) {
            e.remove<type::CurrentFrame>();
                });

        // =========================================================================== 
        // Features
        // =========================================================================== 

        world.type("Feature_Decay")
            .add(decay_system);
    }
}
