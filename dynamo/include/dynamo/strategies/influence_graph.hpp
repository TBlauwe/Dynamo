#ifndef DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP
#define DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP

#include <functional>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <dynamo/internal/process.hpp>

namespace dynamo {

    /**
    An influence is either a positive or negative influence between a behaviour to an object @c T.
    @tparam T Type of the object influenced.
    */
    template<typename T>
    struct Influence
    {
        const T *   object;
        bool        positive;
    };

    /**
    A score is an integer associated with a object of type @c T.
    @tparam T Type of the object associated with a score.
    */
    template<typename T>
    struct Score
    {
        const T *   object;
        int         value;
    };

    /**
    @class InfluenceGraph

    @brief InfluenceGraph
    */
    template<typename T>
    class InfluenceGraph
    {
        using Behaviour_t   = Behaviour<std::vector<Influence<T>>, std::vector<T>>;
        using Influences    = std::unordered_map<const Behaviour_t *, const T *>;
        using Scores        = std::unordered_map<const T *, int>;

    public:
        InfluenceGraph(AgentHandle agent, const std::vector<const Behaviour_t *>& behaviours, std::vector<T> args)
            : _values {args}, _behaviours{behaviours}
        {
            for (const auto& value : _values)
            {
                scores.emplace(std::make_pair(&value, 0));
            }

            for (Behaviour_t const* const behaviour : behaviours)
            {
                for (const auto& influence : (*behaviour)(agent, _values))
                {
                    if (influence.positive)
                    {
                        _positive_influences.emplace(behaviour, influence.object);
                        scores[influence.object] += 1;
                    }
                    else
                    {
                        _negative_influences.emplace(behaviour, influence.object);
                        scores[influence.object] -= 1;
                    }
                }
            }

            std::vector<Score<T>>   sorted_scores {};
            // Sort scores by decreasing order
            for (const auto& [input, score] : scores)
            {
                sorted_scores.emplace_back(input, score);
            }

            std::sort(sorted_scores.begin(), sorted_scores.end(),
                [](const Score<T>& a, const Score<T>& b)
                {
                    return a.value > b.value;
                }
            );

            if (sorted_scores.begin() != sorted_scores.end()) {
                int max_value = sorted_scores[0].value;
                for (const auto& score : sorted_scores)
                {
                    if (score.value == max_value)
                        highest_scores.push_back(const_cast<T*>(score.object));
                }
                
            }
        }

        Influences& positive_influences()
        {
            return _positive_influences;
        }

        Influences& negative_influences()
        {
            return _negative_influences;
        }

        std::vector<const Behaviour_t *>& behaviours()
        {
            return _behaviours;
        }

        std::vector<T>& values()
        {
            return _values;
        }

        bool is_highest(const T* value) const
        {
            return std::find(highest_scores.begin(), highest_scores.end(), value) != highest_scores.end();
        }

        size_t num_eligibles() const
        {
            return highest_scores.size();
        }

        std::vector<T *>& eligibles()
        {
            return highest_scores;
        }

        T result() const
        {
            return *highest_scores[rand()%(num_eligibles() - 1)];
        }

    private:
        Influences      _positive_influences {};
        Influences      _negative_influences {};
        Scores          scores {};
        std::vector<const Behaviour_t *>  _behaviours;
        std::vector<T>  _values;
        std::vector<T *>  highest_scores {};
    };

    namespace strat {

        /**
        An influence graph is a bipartite graph composed of two sets of vertex : U or behaviours and V corresponding to the inputs.
        A set of influences is going from U to V.
        @tparam TInput type of V nodes.
        */
        template<typename TOutput, typename TInput>
        class InfluenceGraph : public Strategy<TOutput, std::vector<Influence<TOutput>>, TInput>
        {
            using Inputs = TInput;
            using BehaviourOutputs = std::vector<Influence<TOutput>>;
            using Behaviour_t = Behaviour<BehaviourOutputs, Inputs>;

        public:

            TOutput compute(AgentHandle agent, const std::vector<Behaviour_t const*> active_behaviours, Inputs inputs) const override
            {
                dynamo::InfluenceGraph<TOutput> graph(agent, active_behaviours, inputs);
                return graph.result();
            }
        };
    }
}

#endif //DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP
