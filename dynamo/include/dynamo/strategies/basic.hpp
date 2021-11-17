#ifndef DYNAMO_STRATEGIES_BASIC_HPP
#define DYNAMO_STRATEGIES_BASIC_HPP

#include <dynamo/internal/process.hpp>

/*
@file dynamo/strategies/basic.hpp


@brief Namespace defining some strategies
*/
namespace dynamo::strat{

    template<typename TOutput, typename ... TInputs>
    class Random : public Strategy<TOutput, TOutput, TInputs ...>
    {
        using Behaviour_t = Behaviour<TOutput, TInputs ...>;
    public:

        TOutput compute(AgentHandle agent, const std::vector<Behaviour_t const*> active_behaviours, TInputs&& ... inputs) const override
        {
            return (* active_behaviours[rand() % active_behaviours.size()])(agent, std::forward<TInputs>(inputs) ...);
        }
    };
}

#endif // DYNAMO_STRATEGIES_BASIC_HPP