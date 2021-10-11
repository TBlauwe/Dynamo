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

        struct Message{
            std::string  message {"Bonjour"};
        };
    }

    namespace module{
        struct GlobalPerception{
            explicit GlobalPerception(flecs::world& world){
                world.module<GlobalPerception>();
                auto& core = dynamo::module_ref<module::Core>(world.import<module::Core>());

                world.system<component::PeriodicEmitter, component::Targets>("PeriodicEmitter")
                        .term<component::Cooldown>().object<component::PeriodicEmitter>().oper(flecs::Not)
                        .arg(1).object(flecs::Wildcard) // <- PeriodicEmitter is actually a pair type with anything
                        .iter([&core](flecs::iter& iter, component::PeriodicEmitter* periodic_emitter, component::Targets* targets) {
                            for(auto i : iter){
                                auto e = iter.entity(i);
                                dynamo::logger(e)->info("Entity {} : {}", e.name().c_str(), iter.term_id(1).object().name());
                                flecs::entity percept = e.world().entity()
                                        .is_a(core.Percept)
                                        .set<dynamo::component::Decay>({2.0f})
                                        .set(iter.term_id(1).object())
                                        .add<dynamo::relation::source>(e);
                                for(flecs::entity_view& entity_view : targets[i].entities){
                                    entity_view.mut(e).add<relation::perceive<senses::Hearing>>(percept);
                                }
                                e.set<component::Cooldown, component::PeriodicEmitter>({periodic_emitter[i].cooldown});
                            }
                        });
            }
        };
    }
}

#endif //DYNAMO_BASIC_PERCEPTION_HPP