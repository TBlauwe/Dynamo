#ifndef DYNAMO_COMPONENTS_HPP
#define DYNAMO_COMPONENTS_HPP

#include <vector>

/**
@file dynamo/internal/components.hpp
@brief Defines some basic components.
*/
namespace dynamo {

	/**
	@brief Namespace regrouping tags with no data.
	*/
	namespace tag {
		/**
		 @brief Tag removed at the end of the current frame. /!\ Beware to handle this tag before flecs::PostFrame /!\ .
		 */
		struct CurrentFrame {};
	}

	namespace component {
		/**
		@brief When ttl (seconds) reaches 0, the entity holding this component is destroyed.
		*/
		struct Decay {
			/**
			 Amount of time left to live in seconds.
			 */
			float ttl;
		};

		/**
		 @brief When remaining_time (seconds) reaches 0, the relation for this cooldown is removed.
		 Must be used as a relation with something else.

		 @code{.cpp}
		 entity.set<component::Cooldown, component::Attack>({1.0f}); // Relation is destroyed in 1 seconds
		 @endcode

		 After cooldown is finished, the relation is removed allowing you to trigger something

		 @code{.cpp}

		 world.system<component::Attack>("System_Attack")
				.term<component::Cooldown>().object<component::Attack>().oper(flecs::Not)	// If @c component::Attack is not in
																							// relation with @c component::Cooldown
				.arg(1).object(flecs::Wildcard)
				.iter([](flecs::iter& iter, component::Attack* attack) {
					for(auto i : iter){
						auto e = iter.entity(i);
						// do something ....
						// and add a cooldown to this entity
						e.set<component::Cooldown, component::Attack>({attack[i].cooldown});
					}
				});
		@endcode
		 */
		struct Cooldown {
			/**
			Amount of time remaining before cooldown is finished (seconds).
			*/
			float remaining_time;
		};

		/**
		@brief Store read-only entities handle. Call mut(...), if you want to modify it.
		*/
		struct Targets {
			/**
			@brief A vector of entities view. Call mut(...), if you need to modify it.
			*/
			std::vector<flecs::entity_view> entities;
		};

		/**
		@brief Component to indicate that the holding entity must reason about something using @T

		@tparam T is the type of a reasonner that should be spawned.
		*/
		template<typename T>
		struct Process{};
	}
}

#endif //DYNAMO_COMPONENTS_HPP
