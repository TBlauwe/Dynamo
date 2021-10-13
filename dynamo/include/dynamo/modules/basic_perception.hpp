#ifndef DYNAMO_BASIC_PERCEPTION_HPP
#define DYNAMO_BASIC_PERCEPTION_HPP

#include <dynamo/internal/core.hpp>
#include <string>

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
                world.import<module::Core>();

                world.system<component::PeriodicEmitter, component::Targets>("PeriodicEmitter")
                        .term<component::Cooldown>().object<component::PeriodicEmitter>().oper(flecs::Not)
                        .arg(1).object(flecs::Wildcard) // <- PeriodicEmitter is actually a pair type with anything
                        .iter([](flecs::iter& iter, component::PeriodicEmitter* periodic_emitter, component::Targets* targets) {
                            for(auto i : iter){
                                auto e = iter.entity(i);
                                auto world = e.world();
                                auto percept = TypeBuilder<type::Percept>(world)
                                        .source<senses::Hearing>(e)
                                        .decay();
                                for(flecs::entity_view& entity_view : targets[i].entities){
                                    percept.perceived_by(entity_view);
                                }
                                e.set<component::Cooldown, component::PeriodicEmitter>({periodic_emitter[i].cooldown});
                            }
                        });
            }
        };
    }
}

#endif //DYNAMO_BASIC_PERCEPTION_HPP