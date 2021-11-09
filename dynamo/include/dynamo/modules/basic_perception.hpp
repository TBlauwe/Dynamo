#ifndef DYNAMO_BASIC_PERCEPTION_HPP
#define DYNAMO_BASIC_PERCEPTION_HPP

#include <dynamo/internal/core.hpp>
#include <string>

namespace dynamo{

    namespace type{
        struct Hearing{};
        struct Smell{};
        struct Taste{};
        struct Touch{};
        struct Vision{};

        /**
         * Component that periodically emits percepts
         */
        struct PeriodicEmitter{
            /**
             * Period
             */
            float cooldown;
        };

        /**
         * Component to represent a message
         */
        struct Message{
            /**
             * Content of the message
             */
            std::string  message {"Bonjour"};
        };
    }

    namespace module{
        /**
         * Module adding perception functionalities.
         */
        struct GlobalPerception{
            /**
             * Module adding perception functionalities.
             */
            explicit GlobalPerception(flecs::world& world){
                world.module<GlobalPerception>();
                world.import<module::Core>();

                world.system<type::PeriodicEmitter, type::Targets>("PeriodicEmitter")
                        .term<type::Cooldown>().obj<type::PeriodicEmitter>().oper(flecs::Not)
                        .arg(1).obj(flecs::Wildcard) // <- PeriodicEmitter is actually a pair type with anything
                        .iter([](flecs::iter& iter, type::PeriodicEmitter* periodic_emitter, type::Targets* targets) {
                            for(auto i : iter){
                                auto e = iter.entity(i);
                                auto world = e.world();
                                auto percept = PerceptBuilder(world).source<type::Hearing>(e);
                                for(flecs::entity_view& entity_view : targets[i].entities){
                                    percept.perceived_by(entity_view);
                                }
                                e.set<type::Cooldown, type::PeriodicEmitter>({periodic_emitter[i].cooldown});
                            }
                        });
            }
        };
    }
}

#endif //DYNAMO_BASIC_PERCEPTION_HPP