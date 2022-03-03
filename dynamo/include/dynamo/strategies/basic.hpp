#pragma once

#include <unordered_set>

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

        TOutput compute(AgentHandle agent, const std::vector<Behaviour_t const*> active_behaviours, TInputs ... inputs) const override
        {
            return (* active_behaviours[rand() % active_behaviours.size()])(agent, std::forward<TInputs>(inputs) ...);
        }
    };

    template<typename TOutput>
    class ContainerAccumulator : public Strategy<TOutput, TOutput>
    {
        using Behaviour_t = Behaviour<TOutput>;
    public:

        TOutput compute(AgentHandle agent, const std::vector<Behaviour_t const*> active_behaviours) const override
        {
            TOutput results{};
            for (auto behaviour : active_behaviours)
            {
                TOutput tmp = (*behaviour)(agent);
                for(const auto value : tmp)
                {
                    if(std::find(results.begin(), results.end(), value) == results.end())
                        results.push_back(value);
                }
            }
            return results;
        }
    };

    template<typename T, typename R = T>
    class Sequential : public Strategy<T, T, R>
    {
        using Behaviour_t = Behaviour<T, R>;
    public:

        T compute(AgentHandle agent, const std::vector<Behaviour_t const*> active_behaviours, R input) const override
        {
            for (auto behaviour : active_behaviours)
            {
                input = (*behaviour)(agent, input);
            }
            return input;
        }
    };

    template<typename ... TInputs>
    class Parallel : public Strategy<void, void, TInputs ...>
    {
        using Behaviour_t = Behaviour<void, TInputs ...>;
    public:

        void compute(AgentHandle agent, const std::vector<Behaviour_t const*> active_behaviours, TInputs ... inputs) const override
        {
            for (auto behaviour : active_behaviours)
            {
                (*behaviour)(agent, std::forward<TInputs>(inputs)...);
            }
        }
    };
}
