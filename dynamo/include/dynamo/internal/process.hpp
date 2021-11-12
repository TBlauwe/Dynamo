#ifndef DYNAMO_PROCESS_HPP
#define DYNAMO_PROCESS_HPP

#include <assert.h>

#include <chrono>
#include <thread>
#include <functional>

#include <range/v3/all.hpp>
#include <taskflow/taskflow.hpp>

#include <dynamo/internal/types.hpp>

/**
@file dynamo/internal/process.hpp
@brief Defines classes related to cognitive reasonning

*/
namespace dynamo {

    //Forward Declaration
    class Agent;

    /**
    @class Behaviour

    @brief A behaviour is a function impacting processes. Their definition is dependant of a @c Strategy.

    @tparam TTOut Type of out data.

    A behaviour is defined by :
    * a name,
    * an activation function,
    * a function.

    An activation function tells whether or not the behaviour should be taken into account.
    The function tells how this behaviour operates.

    A behaviour should not be called manually as it is handled by @c Strategy. See @ref Manual (TODO).

    However, here is a quick overview :

    A behaviour is a functor called liked this :
    @code{.cpp}
    auto behaviour = Behaviour<your::out::type>("MyBehaviour",
        [](Agent agent) -> bool { //... //return true/false },
        [](Agent agent) -> your::out::type { //... //return ... }
        );
    behaviour.is_active(some_agent) // return true or false depending of the agent
    your::out::type result = behaviour(some_agent) //
    @endcode
    */
    template<typename TOutput, typename ... TInputs>
    class Behaviour
    {
    public:
        /**
        @brief Construct a named behaviour with given activation function and function.
        */
        Behaviour(const char* name, std::function<bool(Agent)> activation_callable, std::function<TOutput(Agent, TInputs ...)> callable) :
            _name{ name },
            _activation_callable{ activation_callable },
            _callable{ callable }
        {}

        /**
        @brief Output behaviour's name.
        */
        template<typename T, typename ... U>
        friend std::ostream& operator<<(std::ostream& os, const Behaviour<T, U ...>& behaviour);

        /**
        @brief Returns behaviour's name
        */
        const char* name() const {
            return _name;
        };

        /**
        @brief Tells whether or not this behaviour should be taken into account for the specified agent.
        */
        bool is_active(Agent agent) const {
            return _activation_callable(agent);
        };

        /**
        @brief Compute behaviour for the specified agent.
        */
        TOutput operator()(Agent agent, TInputs ... inputs) const {
            return _callable(agent, inputs ...);
        };

    private:
        const char* _name;
        std::function<TOutput(Agent)> _callable;
        std::function<bool(Agent, TInputs ...)> _activation_callable;
    };

    /**
    @brief Output behaviour's name.
    */
    template<typename T, typename ... U>
    std::ostream& operator<<(std::ostream& os, const Behaviour<T, U ...>& behaviour)
    {
        os << behaviour.name();
        return os;
    }

    /**
    @class Strategy

    @brief A strategy defines how a process should take into account one or multiple behaviours.

    @tparam TTOut Type of out data.

    A strategy defines the signature of a behaviour and how.

    Inherit this class to create your own strategies.

    @code{.cpp}
    YourStrategy<your::out::type> strategy;
    strategy.add(your_behaviour);
    @endcode
    */
    template<typename TOutput, typename TBehaviourOuput = TOutput, typename ... TInputs>
    class Strategy
    {
        using Behaviour_t = Behaviour<TBehaviourOuput, TInputs ...>;

    public:

        /**
        @brief Construct a strategy with no behaviours.
        */
        Strategy() = default;

        /**
        @brief Add behaviour to this strategy.

        @tparam T Behaviour type must correspond to the one accepted by this strategy.
        */
        template<typename T>
        void add(T&& behaviour)
        {
            static_assert(std::is_same_v<T, std::decay_t<Behaviour_t>>, "Wrong type passed, must be a Behaviour<TOut>.");
            behaviours.emplace_back(std::forward<T>(behaviour));
        }

        /**
        @brief Compute strategy with specified agent and specified behaviours. Must at least have one behaviour /!\.

        It will automatically deduced which behaviours should be taken into account before using them.
        */
        TOutput operator()(Agent agent, TInputs ... inputs) const
        {
            assert(behaviours.size() > 0 && "Strategy with no behaviour. Use add().");

            auto active_behaviours = 
                behaviours | 
                ranges::views::filter([&agent](const Behaviour_t& beh) {
                    return beh.is_active(agent);
                }) | 
                ranges::to<std::vector>();

            TBehaviourOuput result = compute(agent, active_behaviours, inputs ...);
            std::cout << agent.name() << " has finished reasonning. The result is : " << result << "\n";
            return result;
        };

        /**
        @brief Pure virtual function telling how this strategy should operate.
        */
        virtual TOutput compute(Agent, std::vector<Behaviour_t>, TInputs ...) const = 0;

    protected:
        std::vector<Behaviour_t> behaviours{};
    };

    /**
    @class Reasonner

    @brief A reasonner is a graph of tasks that represents cognitive processes

    A reasonner must be register before the simulation starts (otherwise, it will not be
    triggered until it is registered).

    To create a reasonner, inherit this class and implement the @c build() function.

    You should not manually create a reasonner. It will be automatically created for you.
    See below :

    @code{.cpp}
    Simulation sim;
    //...
    sim.register_reasonner<YourReasonner>();
    //...
    // For an agent to use your reasonner, use this :
    agent.reason<YourReasonner>();
    //...
    while(run())
    {
        sim.step();
    }

    @endcode
    */
    class Reasonner
    {
    public:
        /**
        @brief Construct a reasonner for the specified agent.
        */
        Reasonner(Agent agent) : agent{ agent } {}

        /**
        @brief Implicit conversion operator to convert it into taskflow.
        */
        inline operator tf::Taskflow && () {
            build();
            return std::move(taskflow);
        }

    protected:

        /**
        @brief Emplace a callable mathing @c std::function<void(Agent)>.

        TODO Wrap taskflow graph creation ?
        */
        template<typename T>
        tf::Task emplace(T&& t)
        {
            return taskflow.emplace([a = this->agent, &t]() {
                std::forward<T>(t)(a);
            });
        };

    private:
        /**
        @brief Pure virtual function used to build a graph of cognitives processes.
        */
        virtual void build() = 0;

    protected:
        Agent agent;
        tf::Taskflow taskflow{};
    };
}
#endif //DYNAMO_PROCESS_HPP
