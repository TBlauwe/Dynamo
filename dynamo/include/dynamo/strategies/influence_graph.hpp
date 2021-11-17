#ifndef DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP
#define DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP

#include <functional>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <dynamo/internal/process.hpp>

namespace dynamo::strat {

/**
An influence is either a positive or negative influence between a behaviour to an object @c T.
@tparam T Type of the object influenced.
*/
template<typename T>
struct Influence
{
    T const * end;
    bool positive;
};

/**
A score is an integer associated with a object of type @c T.
@tparam T Type of the object associated with a score.
*/
template<typename T>
struct Score
{
    T const * data;
    int score;
};

/**
An influence graph is a bipartite graph composed of two sets of vertex : U or behaviours and V corresponding to the inputs.
A set of influences is going from U to V.
@tparam TInput type of V nodes.
*/
template<typename TInput>
class InfluenceGraph : public Strategy<TInput, std::vector<Influence<TInput>>, std::vector<TInput>>
{
    using Inputs            = std::vector<TInput>;
    using BehaviourOutputs  = std::vector<Influence<TInput>>;
    using Behaviour_t       = Behaviour<BehaviourOutputs, Inputs>;

    using Influences        = std::unordered_multimap<Behaviour_t const *, const Influence<TInput>>;
    using Scores            = std::unordered_map<TInput const *, int>;

public:

    TInput compute(AgentHandle agent, const std::vector<Behaviour_t const *> active_behaviours, Inputs&& inputs) const override
    {
        Influences influences{};
        Scores scores{};

        for (const auto& input : inputs)
        {
            auto pair = scores.emplace(std::make_pair(&input, 0));
        }

        // Compute each behaviour 
        for (Behaviour_t const * const behaviour : active_behaviours)
        {
            for (const auto& influence : (*behaviour)(agent, inputs))
            {
                influences.insert(std::make_pair(behaviour, influence));
                if (influence.positive)
                    scores[influence.end] += 1;
                else
                    scores[influence.end] -= 1;
            }
        }

        std::vector<Score<TInput>> sorted_scores {};
        // Sort scores by decreasing order
        for (const auto& [input, score] : scores)
        {
            sorted_scores.emplace_back(input, score);
        }

        std::sort(sorted_scores.begin(), sorted_scores.end(),
            [](const Score<TInput>& a, const Score<TInput>& b)
            {
                return a.score > b.score;
            }
        );

        //if (sorted_scores.begin() != sorted_scores.end()) {
        //    int max_value = sorted_scores.begin()->score;
        //    for (const auto [input, score] : sorted_scores)
        //    {
        //        if (score == max_value)
        //            highest_scores.push_back(influence_score.data);
        //    }
        //    
        //}
        return *(sorted_scores.begin()->data);
    }
};
}

#endif //DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP
