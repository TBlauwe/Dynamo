#ifndef DYNAMO_COMPONENTS_HPP
#define DYNAMO_COMPONENTS_HPP

#include <vector>

namespace dynamo{
    namespace tag{
        /**
         * Tag removed at the end of the current frame. Beware to handle this tag before flecs::PostFrame /!\ .
         */
        struct CurrentFrame{};
    }

    namespace component{
        /**
       * When ttl reaches 0, the entity holding this component is destroyed.
       */
        struct Decay{
            float ttl;
        };

        /**
         * Must be used as a relation with something else.
         * When remaining_time reaches 0, the relation for this cooldown is removed.
         */
        struct Cooldown{
            float remaining_time;
        };

        /**
         * Store read-only entities handle. Call mut(...), if you want to modify it.
         */
        struct Targets{
            std::vector<flecs::entity_view> entities;
        };
    }
}

#endif //DYNAMO_COMPONENTS_HPP
