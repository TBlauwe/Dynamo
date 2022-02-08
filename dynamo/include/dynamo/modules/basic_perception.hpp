#pragma once

#include <dynamo/internal/core.hpp>
#include <string>

namespace dynamo{

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

                world.system<PeriodicEmitter, Targets>("PeriodicEmitter")
                        .term<Cooldown>().obj<PeriodicEmitter>().oper(flecs::Not)
                        .arg(1).obj(flecs::Wildcard) // <- PeriodicEmitter is actually a pair type with anything
                        .iter([](flecs::iter& iter, PeriodicEmitter* periodic_emitter, Targets* targets) {
                            for(auto i : iter){
                                auto e = iter.entity(i);
                                auto world = e.world();
                                auto percept = PerceptBuilder(world).source<Hearing>(e).decay();
                                for(flecs::entity_view& entity_view : targets[i].entities){
                                    percept.perceived_by(entity_view);
                                }
                                e.set<Cooldown, PeriodicEmitter>({periodic_emitter[i].cooldown});
                            }
                        });
            }
        };
    }
}
