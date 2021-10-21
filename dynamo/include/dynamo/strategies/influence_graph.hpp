#ifndef DYNAMO_INFLUENCE_GRAPH_HPP
#define DYNAMO_INFLUENCE_GRAPH_HPP

#include <functional>
#include <algorithm>
#include <vector>

/**
 * An influence is either a positive or negative influence between an object Tu to an object Tv.
 * @tparam Tu Type of the object influencing.
 * @tparam Tv Type of the object influenced.
 */
template<typename Tu, typename Tv>
struct Influence
{
    const Tu*	start;
    const Tv*	end;
    bool		positive;
};

/**
 * A score is an integer associated with a object of type Tv.
 * @tparam Tv Type of the object associated with a score.
 */
template<typename Tv>
struct Score
{
    const Tv* data;
    int score;
};

/**
 * An influence graph is a bipartite graph composed of two sets of vertex : U and V or left and right.
 * A set of influences is coming from U to V.
 * @tparam Tu type of U nodes.
 * @tparam Tv type of V nodes.
 */
template<typename Tu, typename Tv>
class InfluenceGraph
{
public :
    std::vector<Tu>	U {};
    std::vector<Tv>	V {};

    std::vector<Influence<Tu, Tv>>	positive_influences {};
    std::vector<Influence<Tu, Tv>>	negative_influences {};
    std::vector<Score<Tv>>			scores {};
    std::vector<const Tv*>			highest_scores {};

public:
    InfluenceGraph() = default;
    InfluenceGraph(const std::vector<Tu>& u_vec, const std::vector<Tv>& v_vec, std::function<std::vector<Influence<Tu, Tv>>(const Tu&, const std::vector<Tv>&)> compute) :
            U{u_vec},
            V{v_vec}
    {
        std::unordered_map<const Tv*, int> _scores;
        for (const Tv& v : V)
        {
            _scores.emplace(&v, 0);
        }

        for (const Tu& u : U)
        {
            for (Influence<Tu, Tv>& influence : compute(u, V))
            {
                if (influence.positive)
                {
                    positive_influences.emplace_back(&u, influence.end, true);
                    _scores[influence.end] += 1;
                }
                else
                {
                    negative_influences.emplace_back(&u, influence.end, false);
                    _scores[influence.end] -= 1;
                }
            }
        }

        for (const auto& [v, score] : _scores)
        {
            scores.emplace_back(v, score);
        }

        std::sort(scores.begin(), scores.end(),
                  [](const Score<Tv>& a, const Score<Tv>& b)
                  {
                      return a.score > b.score;
                  }
        );

        if (scores.begin() != scores.end()) {
            int max_value = scores.begin()->score;
            for (const auto& influence_score : scores)
            {
                if (influence_score.score == max_value)
                    highest_scores.push_back(influence_score.data);
            }
        }
    }

    inline std::vector<Score<Tv>> get_scores() { return scores; };
    inline std::vector<const Tv*> get_highest_scores(){ return highest_scores; };
};

#endif //DYNAMO_INFLUENCE_GRAPH_HPP
