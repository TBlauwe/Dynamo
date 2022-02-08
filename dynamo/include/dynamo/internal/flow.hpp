#pragma once

#include <assert.h>
#include <chrono>
#include <thread>
#include <functional>
#include <typeinfo>

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
        [](Agent agent, TInputs ...) -> TBehaviourOutput {return ...; }
});

@endcode
*/
namespace dynamo {

    //Forward Declaration
    class AgentHandle;

    template<typename T>
    using TaskMap = std::unordered_map<size_t, T>;

    enum class ProcessType : int
    {
        Simple = 0,
        Composed,
        Conditional,
        Static,
        Not_a_process,
        Undefined
    };

    /**
    @brief convert a task type to a human-readable string
    */
    inline const char* to_string(ProcessType type) {
        switch (type) {
        case ProcessType::Simple:           return "Simple";
        case ProcessType::Composed:         return "Composed";
        case ProcessType::Conditional:      return "Conditional";
        case ProcessType::Static:           return "Static";
        case ProcessType::Not_a_process:    return "Not_a_process";
        default:                            return "Undefined";
        }
    }
    
    /**
    @class ProcessBase

    @brief A process is a small addon to a task to store information for visualization.
    */
    class ProcessBase
    {
        friend class FlowBuilder;

        template <typename U>
        friend class Process;

    public:
        /**
        @brief Construct a process associated with provided task.
        A process is a small addon to a task to store information for visualization.
        */
        ProcessBase(tf::Task task, std::type_index strategy_index, ProcessType type = ProcessType::Undefined) : 
            _task{ task }, _strategy_index{ strategy_index }, _type{type} {}

        /**
        @brief Returns underlying task.
        */
        inline tf::Task task() { return _task; }

        /**
        @brief Returns underlying task.
        */
        inline ProcessType type() const { return _type; }

        /**
        @brief Returns underlying task hash value.
        */
        inline size_t hash_value() { return _task.hash_value(); }

        /**
        @brief Set process' name (Use for debugging/visualization).
        */
        inline void name(const char* name) { _task.name(name); }
        
        /**
        @brief Get process' name.
        */
        inline const char * name() const { return _task.name().c_str(); }

        /**
        @brief Add a dependency with no inputs.
        */
        inline void succeed(tf::Task t) { _task.succeed(t); }

        /**
        @brief Number of inputs coming from other processes. Doesn't count dependencies with no inputs.
        */
        inline size_t number_of_inputs() const { return _input_names.size(); }

        /**
        @brief Set input's name for specified process.
        */
        template <typename U>
        inline void input_name(Process<U>& p, const char* name) 
        { 
            _input_names[p.hash_value()] = name;
        }

        /**
        @brief Get input's name for specified task.
        */
        inline const char * input_name(size_t hash) const
        {
            return _input_names.at(hash);
        }

        inline bool has_input_from(size_t hash) const
        {
            return _input_names.contains(hash);
        }

        inline TaskMap<const char *> input_names()
        {
            return _input_names;
        }
        
        std::type_index strategy_index() { return _strategy_index; }

    protected:

        template <typename U>
        void succeed(Process<U>& p)
        {
            succeed(p.task());
            _input_names.emplace(p.hash_value(), typeid(U).name()); // By default, try to deduce type name at runtine.
        }

    private:
        ProcessType             _type;
        tf::Task                _task;
        std::type_index         _strategy_index;
        TaskMap<const char *>   _input_names {};
    };

    /**
    @class Process
    @brief A process is a glorified function that has multiple inputs and one output (specified by @c T).
    */
    template<typename T>
    class Process
    {
        friend class FlowBuilder;
    public:
        Process(ProcessBase& process, std::shared_ptr<T> result) :
            process{ process }, result {result}
        {}

        /**
        @brief Returns underlying task.
        */
        inline tf::Task task() { return process.task(); }

        /**
        @brief Returns underlying task hash value.
        */
        inline size_t hash_value() { return process.hash_value(); }

        /**
        @brief Set process' name (Use for debugging/visualization).
        */
        inline void name(const char* name) { process.name(name); }

        /**
        @brief Get process' name.
        */
        inline const char* name() const { return process.name().c_str(); }

        /**
        @brief Add a dependency with no inputs.
        */
        inline void succeed(tf::Task& t) { process.succeed(t); }

        /**
        @brief Set input's name for specified process.
        */
        template <typename U>
        inline void input_name(Process<U>& p, const char* name)
        {
            process.input_name(p, name);
        }

        /**
        @brief Return underlying shared_ptr.
        */
        inline std::shared_ptr<T> output()
        {
            return result;
        }

    protected:

        template <typename U>
        void succeed(Process<U>& p)
        {
            process.succeed(p);
        }

    private:
        ProcessBase&        process;
        std::shared_ptr<T>  result;
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
        Behaviour(const char* name, std::function<bool(AgentHandle)> activation_callable, std::function<TOutput(AgentHandle, const TInputs& ...)> callable) :
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
        TOutput operator()(AgentHandle agent, const TInputs& ... inputs) const {
            return _callable(agent, inputs...);
        };

    private:
        const char* _name;
        std::function<TOutput(AgentHandle, const TInputs& ...)> _callable;
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
    @class FlowBuilder

    @brief A flow is a graph of tasks to represent cognitive processes

    A flow must be registed before the simulation starts (otherwise, it will not be
    triggered, until it is registered).

    To create a flow, inherit this class and implement the @c build() function.

    You should not manually create a flow. It will be automatically created for you.
    See below :

    @code{.cpp}
    Simulation sim;
    //...
    sim.flow<YourFlow>();
    //...
    // For an agent to use your flow, use this :
    agent.flow<YourFlow>();
    //...
    while(run())
    {
        sim.step();
    }

    @endcode
    */

    class FlowBuilder
    {
    public:
        /**
        @brief Construct an agent model for the specified agent.
        */
        FlowBuilder(Strategies const * const  strategies, AgentHandle agent) : strategies{ strategies }, agent { agent } {}

        /**
        @brief Pure virtual function used to build a graph of cognitives processes.
        */
        virtual void build() = 0;

        /**
        @brief Implicit conversion operator to convert it into taskflow.
        */
        inline operator tf::Taskflow && ()
        {
            taskflow.name(name());
            return std::move(taskflow);
        }

        /**
        @brief Pure virtual function to return the name of this flow (for visualization).
        */
        virtual constexpr const char* name() const = 0;

        TaskMap<ProcessBase> process_details() const
        {
            return task_to_process;
        }

    protected:

        /**
        @brief Emplace a callable mathing @c std::function<void(Agent)>.

        TODO Wrap taskflow graph creation ?
        */
        template<typename T>
        tf::Task emplace(T&& t)
        {
            auto task = taskflow.placeholder();
            task_to_process.emplace(task.hash_value(), ProcessBase{ task, typeid(void), ProcessType::Not_a_process }).first->second;
            task.work([a = this->agent, &t]() {
                std::forward<T>(t)(a);
            });
            return task;
        };

        /**
        @brief Emplace a process.
        */
        template<template<typename, typename ...> typename T, typename TOutput, typename ... TInputs>
        Process<TOutput> process(Process<TInputs>& ... inputs)
        {
            auto task       = taskflow.placeholder();
            ProcessBase& pb = task_to_process.emplace(task.hash_value(), ProcessBase{task, typeid(T<TOutput, TInputs...>), ProcessType::Simple}).first->second;
            (pb.succeed(inputs), ...);
            
            auto output     = std::make_shared<TOutput>();
            Process<TOutput> p (pb, output);

            task.work(
                [strat = this->strategies, a = this->agent, ... args = inputs.result, res = std::move(output)]() mutable
                {
                    *res = strat->get<T<TOutput, TInputs...>>()(a, *args ...);
                }
            );

            return p;
        };

        template<typename T, typename ... Args>
        Process<T> static_value(Args&& ... args)
        {
            auto task       = taskflow.placeholder();
            ProcessBase& pb = task_to_process.emplace(task.hash_value(), ProcessBase{task, typeid(T), ProcessType::Static}).first->second;
            
            auto output = std::make_shared<T>(T{ std::forward<Args>(args)... });
            Process<T> p (pb, output);

            // Empty task that is just mainting a counter for the shared ptr.
            task.work([res = std::move(output)]() mutable {});

            return p;
        }

    private:

        Strategies const * const strategies;
        AgentHandle     agent;
        tf::Taskflow    taskflow {};
        TaskMap<ProcessBase> task_to_process;
    };

    namespace type
    {
        /**
        @brief Component with a map to retrieve the process corresponding to a task.
        */
        struct ProcessDetails
        {
            /**
            @brief Map of task's hash to a process (encapsulated in an any, as there multiple type of processes).
            */
            TaskMap<ProcessBase> container;

            ProcessBase find(size_t hash) const
            {
                return container.at(hash);
            }
        };
    }
}
