#ifndef DYNAMO_BASIC_ACTION_HPP
#define DYNAMO_BASIC_ACTION_HPP

#include <flecs.h>

namespace dynamo{

    namespace type{
        struct Ordered{};
        struct Arity
        {
            int value{ 1 };
        };

        struct Cost{
            int value {0};
        };

        struct ReglementaryCost{
            int  value {0};
        };

        struct Qualification{
            int  value {1};
        };
    }

    inline bool is_cooperative(flecs::entity& e)
    {
        return e.has<type::Arity>() && e.get<type::Arity>()->value > 1;
    }

    inline bool is_mastered(flecs::entity& agent, flecs::entity& action)
    {
        return agent.get<type::Qualification>()->value >= action.get<type::Qualification>()->value;
    }

    namespace module{
        /**
         * Module adding perception functionalities.
         */
        struct BasicAction{
            /**
             * Module adding perception functionalities.
             */
            explicit BasicAction(flecs::world& world){
                world.module<BasicAction>();
            }
        };
    }
}

#endif //DYNAMO_BASIC_PERCEPTION_HPP
