#include <dynamo/internal/core.hpp>

namespace dynamo {
    module::Core::Core(flecs::world& world) {
        world.module<Core>();

        // =========================================================================== 
        // Components
        // =========================================================================== 

        auto e = world.component<perceive>();
        e.add(flecs::Transitive);

        // =========================================================================== 
        // Pipeline
        // =========================================================================== 

        auto decay_system = world.system<Decay>("Decay")
            .kind(flecs::PreFrame)
            .each([](flecs::entity e, Decay& decay) {
            if (decay.ttl <= 0.f) {
                e.destruct();
            }
            else {
                decay.ttl -= e.delta_time();
            }
                });

        world.system<Cooldown>("Cooldown linked")
            .arg(1).obj(flecs::Wildcard) // <- Cooldown is actually a pair type with anything
            .kind(flecs::PreFrame)
            .iter([](flecs::iter& iter, Cooldown* cooldown) {
            for (auto i : iter) {
                cooldown[i].remaining_time -= iter.delta_time();
                if (cooldown[i].remaining_time <= 0) {
                    iter.entity(i).remove<Cooldown>(iter.id(1).object());
                }
            }
                });

        world.system<Cooldown>("Cooldown single")
            .kind(flecs::PreFrame)
            .iter([](flecs::iter& iter, Cooldown* cooldown) {
            for (auto i : iter) {
                cooldown[i].remaining_time -= iter.delta_time();
                if (cooldown[i].remaining_time <= 0) {
                    iter.entity(i).remove<Cooldown>();
                }
            }
        });

        world.system<CurrentFrame>("RemoveCurrentFrameTag")
            .kind(flecs::PostFrame)
            .each([](flecs::entity e, CurrentFrame) {
            e.remove<CurrentFrame>();
                });

        // =========================================================================== 
        // Features
        // =========================================================================== 

        world.type("Feature_Decay")
            .add(decay_system);
    }
}
