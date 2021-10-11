#include <dynamo/modules/core.hpp>

namespace dynamo{
    module::Core::Core(flecs::world &world) {
        world.module<Core>();

        agents_query = world.query<dynamo::type::Agent>();

        // ========== Trigger ==========

        world.system<const component::Decay>("OnSet_Decay_RememberInitialValue")
                .kind(flecs::OnSet)
                .each([](flecs::entity e, const component::Decay& decay) {
                    e.set<component::InitialValue<component::Decay>>({{decay.ttl}});
                });

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

        world.system<component::Cooldown>()
                .arg(1).object(flecs::Wildcard) // <- Cooldown is actually a pair of type with anything
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

        // ========== Prefab ==========
        //TODO when going to flecs v3 change add_owned by override

        Action = world.prefab("action_prefab")
                .add<type::Action>()
                .add_owned<type::Action>()
                ;

        Agent = world.prefab("agent_prefab")
                .add<type::Agent>()
                .add_owned<type::Agent>()
                ;

        Artefact = world.prefab("artefact_prefab")
                .add<type::Artefact>()
                .add_owned<type::Artefact>()
                ;

        Organisation = world.prefab("organisation_prefab")
                .add<type::Organisation>()
                .add_owned<type::Organisation>()
                ;

        Percept = world.prefab("percept_prefab")
                .add<type::Percept>()
                .add_owned<type::Percept>()
                ;

    }
}
