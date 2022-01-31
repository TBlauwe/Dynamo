#pragma once

#include <flecs.h>

namespace dynamo{

    namespace type{
        struct Ordered{};
        struct Arity
        {
            int value{ 1 };
        };

        struct PreCondition_Contextual
        {
            int  value {1};
        };

        struct PreCondition_Nomological
        {
            int  value {1};
        };

        struct PreCondition_Reglemantary
        {
            int  value {1};
        };

        struct PreCondition_Favorable
        {
            int  value {1};
        };

        struct Qualification
        {
            int  value {1};
        };

        struct Root{};
        struct Task{};

        struct feasible {};
    }

    enum class LogicalConstructor : int
    {
        AND = 0,
        OR,
        Undefined
    };

    /**
    @brief convert a logical constructor to a human-readable string
    */
    inline const char* to_string(LogicalConstructor val)
    {
        switch (val)
        {
			case LogicalConstructor::AND:   return "AND";
			case LogicalConstructor::OR:    return "OR";
			default: return "Undefined";
        }
    }

    enum class TemporalConstructor : int
    {
        SEQ = 0,
        SEQ_ORD,
        IND,
        Undefined
    };

    /**
    @brief convert a temporal constructor to a human-readable string
    */
    inline const char* to_string(TemporalConstructor val)
    {
        switch (val) 
        {
			case TemporalConstructor::IND:      return "IND";
			case TemporalConstructor::SEQ:      return "SEQ";
			case TemporalConstructor::SEQ_ORD:  return "SEQ-ORD";
			default: return "Undefined";
        }
    }

    inline bool is_cooperative(const flecs::entity& e)
    {
        return e.has<type::Arity>() && e.get<type::Arity>()->value > 1;
    }

    inline bool is_mastered(const flecs::entity& agent, const flecs::entity& action)
    {
        return agent.has<type::Qualification>() && (agent.get<type::Qualification>()->value >= action.get<type::Qualification>()->value);
    }

    namespace module{
        /**
         * Module adding activity-dl functionalities.
         */
        struct ADL
        {
            explicit ADL(flecs::world& world)
            {
                world.module<ADL>();
            }
        };
    }
}
