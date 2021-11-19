#ifndef DYNAMO_PROCESS_HPP
#define DYNAMO_PROCESS_HPP

#include <assert.h>

#include <chrono>
#include <thread>
#include <functional>

#include <taskflow/taskflow.hpp>

#include <dynamo/internal/types.hpp>
#include <dynamo/utils/containers.hpp>

/**
@file dynamo/internal/process.hpp
@brief Defines classes related to cognitive reasonning.

Broadly speaking, an agent model defines how an agent operates. It continuosly receives @Percept and
may act accordingly.

Here, we define an agent model as an assemblage of process, or function taking inputs and returning an output.
A @c Behaviour impact theses processes. To take them into account, we define
a @Strategy. It tells how to handle multiple behaviours for this particular process.

TODO resume/improve documentation (link to articles ?)

Usage : 

@code{.cpp}
// I - Define your strategies or use exisiting one
template<typename TOutput, TBehaviourOutputs, typename ... TInputs>
class MyStrategy : public Strategy<TOutput, TBehavioursOutputs, TInputs ...>
{
    using Behaviour_t = Behaviour<TBehaviourOutput, TInputs...>; // For convenience
public:

    TOutput compute(Agent agent, std::vector<Behaviour_t> active_behaviours, TInputs ... inputs) const override
    {
        // implement your strategy here
        return // your result;
    }
};

// II - Define an agent model that may use your strategy

TODO

// III - Register the strategies you'll be using
Simulation sim;
auto my_strat = sim.add<MyStrategy>(); // Don't call it multiples times !!!

// III - Register your agent model. As they will use strategies, these must be registered before your agent model runs. 
// So not necessarily before register your models, although it is safer.
sim.register_reasonner<MyReasonner>();

// IV - Add behaviours to your strategy
auto my_strat = sim.get<MyStrategy>();
random_strat.add(Behaviour<TBehaviourOutput, ... TInputs>{
    "MyFirstBehaviour",
        [](Agent agent) -> bool {return true; },
        [](Agent agent, TInputs ...) -> TBehaviourOutput {return "Yeah"; }
});

@endcode
*/
namespace dynamo {

    //Forward Declaration
    class AgentHandle;
    
    /**
    @class Process
    @brief A process is a glorified function that has multiple inputs and one output (specified by @c T).
    */
    template<typename T>
    class Process
    {
        friend class AgentModel;
        template <typename U>
        friend class Process;

    public:
        Process(tf::Taskflow& taskflow) : task{taskflow.placeholder()} {}
        Process(tf::Taskflow& taskflow, T&& output) : task{ taskflow.placeholder() }
        {
            *result = std::forward<T>(output);
        }

        /**
        @brief Set name of the process (Use for debugging/visualization).
        */
        void name(const char* name)
        {
            task.name(name);
        }
        
        /**
        @brief Get name of the process.
        */
        const char * name()
        {
            return task.name().c_str();
        }

        /**
        @brief After @c t is finished, this process can run (provided other dependencies are finished).
        */
        void succeed(tf::Task& t)
        {
            task.succeed(t);
        }

    private:

        template <typename U>
        void succeed(Process<U>& p)
        {
            succeed(p.task);
        }

    private:
        std::shared_ptr<T> result{ std::make_shared<T>() };
        tf::Task task;
    };

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
        Behaviour(const char* name, std::function<bool(AgentHandle)> activation_callable, std::function<TOutput(AgentHandle, TInputs ...)> callable) :
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
        bool is_active(AgentHandle agent) const {
            return _activation_callable(agent);
        };

        /**
        @brief Compute behaviour for the specified agent.
        */
        TOutput operator()(AgentHandle agent, TInputs&& ... inputs) const {
            return _callable(agent, std::forward<TInputs>(inputs) ...);
        };

    private:
        const char* _name;
        std::function<TOutput(AgentHandle, TInputs&& ...)> _callable;
        std::function<bool(AgentHandle)> _activation_callable;
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
        using Strategy_t = Strategy<TOutput, TBehaviourOuput, TInputs ...>;
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
        template<typename ... Args>
        Strategy_t& behaviour(Args&&... args)
        {
            behaviours.emplace_back(args...);
            return *this;
        }

        /**
        @brief Compute strategy with specified agent and specified behaviours. Must at least have one behaviour /!\.

        It will automatically deduced which behaviours should be taken into account before using them.
        */
        TOutput operator()(AgentHandle agent, TInputs ... inputs) const
        {
            assert(behaviours.size() > 0 && "Strategy with no behaviour. Add behaviour with your_strat.behaviour(args..).");
            return compute(agent, active_behaviours(agent), inputs ...);
        }

        /**
        @brief Pure virtual function telling how this strategy should operate.
        */
        virtual TOutput compute(AgentHandle, const std::vector<Behaviour_t const *>, TInputs ...) const = 0;

    protected:
        std::vector<Behaviour_t> behaviours{};



    private:
        std::vector<Behaviour_t const *> active_behaviours(AgentHandle agent) const
        {
            std::vector<Behaviour_t const *> container{};
            for (const auto& behaviour : behaviours)
            {
                if (behaviour.is_active(agent))
                    container.emplace_back(&behaviour);
            }
            assert(container.size() > 0 && "Strategy with no active behaviour !");
            return container;
        }
    };

    using Strategies = TypeMap;
    /**
    @class AgentModel

    @brief An agent model is a graph of tasks/computation to represent cognitive processes

    An agent model must be register before the simulation starts (otherwise, it will not be
    triggered until it is registered).

    To create an agent model, inherit this class and implement the @c build() function.

    You should not manually create an agent. It will be automatically created for you.
    See below :

    @code{.cpp}
    Simulation sim;
    //...
    sim.agent_model<YourAgentModel>();
    //...
    // For an agent to use your agent model, use this :
    agent.reason<YourAgentModel>();
    //...
    while(run())
    {
        sim.step();
    }

    @endcode
    */
    class AgentModel
    {
    public:
        /**
        @brief Construct an agent model for the specified agent.
        */
        AgentModel(Strategies const * const  strategies, AgentHandle agent) : strategies{ strategies }, agent { agent } {}

        /**
        @brief Implicit conversion operator to convert it into taskflow.
        */
        inline operator tf::Taskflow && ()
        {
            build();
            taskflow.name(name());
            return std::move(taskflow);
        }

        /**
        @brief Dump the graph to a DOT format.
        */
        std::string to_graphviz() const
        {
            return taskflow.dump();
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

        /**
        @brief Emplace a process.
        */
        template<template<typename, typename ...> typename T, typename TOutput, typename ... TInputs>
        Process<TOutput> process(Process<TInputs>& ... inputs)
        {
            Process<TOutput> p(taskflow);
            p.task.work([strat = this->strategies, a = this->agent, ... args = inputs.result, res = p.result]() mutable
            {
                *res = strat->get<T<TOutput, TInputs...>>()(a, *args ...);
                }),
  
            (p.succeed(inputs), ...);
            return p;
        };

        template<typename TOutput>
        Process<TOutput> static_value(TOutput&& output)
        {
            return Process<TOutput>(taskflow, std::forward<TOutput>(output));;
        }

    private:
        /**
        @brief Pure virtual function used to build a graph of cognitives processes.
        */
        virtual void build() = 0;

        /**
        @brief Pure virtual function to return the name of this flow (for visualization).
        */
        virtual constexpr const char * name() const = 0;

        Strategies const * const strategies;
        AgentHandle agent;
        tf::Taskflow taskflow {};
    };
}
#endif //DYNAMO_PROCESS_HPP
