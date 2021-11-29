#ifndef DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP
#define DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP

#include <functional>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <dynamo/algorithm/influence_graph.hpp>
#include <dynamo/internal/core.hpp>

namespace dynamo::type
{
    template<typename T>
    struct IGOutput
    {
        dynamo::InfluenceGraph<T> graph;
    };
}
namespace dynamo::strat
{
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
            agent.set<dynamo::type::IGOutput<TOutput>>({ graph });
            return graph.result();
        }
    };
}

#endif //DYNAMO_STRATEGIES_INFLUENCE_GRAPH_HPP
