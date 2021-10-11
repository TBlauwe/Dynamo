#include <dynamo/modules/core.hpp>

namespace dynamo{
    // Do not add systems that are not in the pipeline
    flecs::type create_feature(flecs::world& world, const char *name, const char * description) {
        auto feature = world.type(fmt::format("Feature_{}", name).c_str());
        return feature;
    }

    module::Core::Core(flecs::world &world) {
        world.module<Core>();

        Feature = world.entity("Feature");
        agents_query = world.query<dynamo::type::Agent>();

        // ========== Trigger ==========

        world.system<const type::Event>("OnAdd_Event_AddCurrentFrameTag")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const type::Event event) {
                    e.set<component::Tick>({e.world().get_tick()});
                    e.add<tag::CurrentFrame>();
                });

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

        // ========== Prefab ==========

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

        Event = world.prefab("event_prefab")
                .add<type::Event>()
                .add_owned<type::Event>()
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
