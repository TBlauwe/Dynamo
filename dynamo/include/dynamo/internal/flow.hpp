#pragma once

#include <assert.h>
#include <chrono>
#include <thread>
#include <functional>
#include <typeinfo>

#include <taskflow/taskflow.hpp>

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

    enum class Role : int
    {
        Simple = 0,
        Acquisition,
        Intepretation,
        Manipulation,
        Transform,
        Storage,
        Composed,
        Conditional,
        Value,
        Undefined
    };

    /**
    @brief convert a role to a human-readable string
    */
    inline const char* to_string(Role type) {
        switch (type) {
        case Role::Simple:           return "Simple";
        case Role::Acquisition:      return "Acquisition";
        case Role::Intepretation:    return "Interpretation";
        case Role::Manipulation:     return "Manipulation";
        case Role::Transform:        return "Transform";
        case Role::Storage:          return "Storage";
        case Role::Composed:         return "Composed";
        case Role::Conditional:      return "Conditional";
        case Role::Value:            return "Value";
        default:                     return "Undefined";
        }
    }

    /**
    @class Instruction

    @brief An instruction is a function called by an @c Algorithm. Its definition is dependant of the @c Algorithm used.

    @tparam TAgent Agent type
    @tparam TOutput Instruction output type
    @tparam TInputs Instruction inputs type

    An instruction is defined by :
    * a name,
    * an activation function,
    * a function.

    An activation function tells whether or not the behaviour should be taken into account.
    The function tells how this behaviour operates.

    An instruction should not be called manually as it is handled by @c Algorithm. See @ref Manual (TODO).

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
    template<typename TAgent, typename TOutput, typename ... TInputs>
    class Instruction
    {
    public:
        /**
        @brief Construct a named instruction with given activation function and function.
        */
        Instruction(const char* name, std::function<bool(TAgent)> activation_callable, std::function<TOutput(TAgent, const TInputs& ...)> callable) :
            _name{ name },
            _activation_callable{ activation_callable },
            _callable{ callable }
        {}

        /**
        @brief Output instruction's name.
        */
        template<typename A, typename U, typename ... V>
        friend std::ostream& operator<<(std::ostream& os, const Instruction<A, U, V ...>& behaviour);

        /**
        @brief Returns instruction's name
        */
        const char* name() const {
            return _name;
        };

        /**
        @brief Tells whether or not this instruction should be taken into account for the specified agent.
        */
        bool is_active(TAgent agent) const {
            return _activation_callable(agent);
        };

        /**
        @brief Compute instruction for the specified agent.
        */
        TOutput operator()(TAgent agent, const TInputs& ... inputs) const {
            return _callable(agent, inputs...);
        };

    private:
        const char* _name;
        std::function<bool(TAgent)> _activation_callable;
        std::function<TOutput(TAgent, const TInputs& ...)> _callable;
    };

    /**
    @brief Output instruction's name.
    */
    template<typename TAgent, typename TOuput, typename ... TInputs>
    std::ostream& operator<<(std::ostream& os, const Instruction<TAgent, TOuput, TInputs ...>& instruction)
    {
        os << instruction.name();
        return os;
    }


    /**
    @class Algorithm 

    @brief An algorithm defines how a list of modular instructions should be processed.

    @tparam TTOut Type of out data.

    Inherit this class to create your own strategies.

    @code{.cpp}
    YourStrategy<your::out::type> strategy;
    strategy.add(your_behaviour);
    @endcode
    */
    template<typename TAgent, typename TOutput, typename TInstructionOutput, typename ... TInputs>
    class Algorithm 
    {
    public:

        using agent_type            = TAgent;
        using Algo                  = Algorithm<TAgent, TOutput, TInstructionOutput, TInputs ...>;
        using Instruction_t         = Instruction<TAgent, TInstructionOutput, TInputs ...>;
        using ActiveInstructions    = std::vector<Instruction_t const*>;

        Algorithm() = default;

        /**
        @brief Add an instruction to this strategy.

        @tparam T Behaviour type must correspond to the one accepted by this strategy.
        */
        template<typename ... Args>
        Algorithm& add_instruction(Args&&... args)
        {
            instructions.emplace_back(std::forward<Args>(args)...);
            return *this;
        }


        /**
        @brief Compute with specified agent and instructions. Must at least have one instructions /!\.

        Only active instructions will be processed.
        */
        TOutput operator()(TAgent agent, TInputs ... inputs) const
        {
            assert(instructions.size() > 0 && "Algorithm has no instruction. Add instruction with : algorithm.instruction(...).");
            return compute(agent, active_instructions(agent), inputs ...);
        }

        /**
        @brief Pure virtual function telling how this algorithm should operate.
        */
        virtual TOutput compute(TAgent, const ActiveInstructions, TInputs ...) const = 0;

    private:
        std::vector<Instruction_t> instructions;

        /**
        @brief Returns all active instructions.
        */
        ActiveInstructions active_instructions(TAgent agent) const
        {
            ActiveInstructions container{};
            for (const auto& instruction : instructions)
            {
                if (instruction.is_active(agent))
                    container.emplace_back(&instruction);
            }
            assert(container.size() > 0 && "Algorithm has no active instruction !");
            return container;
        }
    };

    template<typename TAgent, typename TOutput, typename TInstructionOutput>
    using Acquisition = Algorithm<TAgent, TOutput, TInstructionOutput>;

    template<typename TAgent, typename TOutput, typename TInstructionOutput, typename ... TInputs>
    using Interpretation = Algorithm< TAgent, TOutput, TInstructionOutput, TInputs ...> ;

    template<typename TAgent>
    using Manipulation = Algorithm<TAgent, void, void> ;

    template<typename TAgent, typename T, typename ... TInputs>
    using Transform = Algorithm<TAgent, T, T, TInputs ...> ;

    template<typename TAgent, typename ... TInputs>
    using Storage = Algorithm<TAgent, void, void, TInputs ...> ;

    using Algorithms = std::unordered_map<std::string, std::any>;


    /**
    @class Operation 

    @brief An operation is a function that can be chained to other operation.
    */
    class Operation 
    {
    public:
        /**
        @brief Construct a process associated with provided task.
        A process is a small addon to a task to store information for visualization.
        */
        Operation(tf::Task task, Role role = Role::Undefined) : 
            _task{ task }, _role{role} {}

        /**
        @brief Returns underlying task.
        */
        inline Role role(Role r) { _role = r; }

        /**
        @brief Returns underlying task.
        */
        inline Role role() const { return _role; }

        /**
        @brief Set process' name (Use for debugging/visualization).
        */
        inline void name(const char* name) { _task.name(name); }
        
        /**
        @brief Get process' name.
        */
        inline const std::string name() const { return _task.name(); }

        /**
        @brief Execute this operation after the specified operation.
        */
        void succeed(Operation op)
        {
            succeed(op.task());
        }

    protected:
        /**
        @brief Execute this operation after the specified task.
        */
        inline void succeed(tf::Task t) { _task.succeed(t); }

        /**
        @brief Returns underlying task.
        */
        inline tf::Task task() { return _task; }

        /**
        @brief Returns underlying task hash value.
        */
        inline size_t hash_value() { return _task.hash_value(); }

    private:
        Role                _role;
        tf::Task            _task;
    };

    template<typename T>
    struct DataFlow 
    {
        tf::Task            task {};
        std::shared_ptr<T>  value {};
    };

    template<typename T>
    using OperationOutput = std::pair<Operation, DataFlow<T>>;


    template<typename TAgent>
    class Flow
    {
    public:
        // Construction
        // ============
        Flow(Algorithms * const * const algorithms, TAgent& agent) : algorithms{algorithms}, agent { agent }{};

        /**
        @brief Set process' name (Use for debugging/visualization).
        */
        inline Flow& name(const char* name) { taskflow.name(name); }

        /**
        @brief Get process' name.
        */
        inline const std::string name() const { return taskflow.name(); }

        /**
        @brief Remove all operations
        */
        inline void clear() { taskflow.clear(); }

        // Assembly
        // ========

        /**
        @brief Emplace a callable mathing @c std::function<void(Agent)>.
        */
        template<typename Callable>
        tf::Task emplace(Callable&& callable)
        {
            auto task = taskflow.emplace
            (
				[a = this->agent, c = std::forward<Callable>(callable)]()
                {
					c(a);
				}
            );
            return task;
        };


        /**
        @brief Emplace a callable mathing @c std::function<void(Agent)>.
        */
        template<typename TName, typename TAlgorithm, typename ... TInputs>
        Operation storage(DataFlow<TInputs> ... inputs)
        { 
            //static_assert(std::is_base_of<Storage<TAgent, TInputs...> , TAlgorithm> ::value, "Derived not derived from BaseClass");
            static_assert(std::is_same<TAgent, TAlgorithm::agent_type>::value, "Algorithm and Flow do not manipulate same agent type");

            auto task       = taskflow.placeholder();

            Operation operation {task, Role::Storage};
            (operation.succeed(inputs), ...);
            
            task.work(
                [algos = this->algorithms, a = this->agent, ... args = inputs.value]()
                {
                    algos->get<TName, TAlgorithm>()(a, *args ...);
                }
            );

            return operation;
        };

        template<typename TName, typename TAlgorithm, typename TOutput>
        OperationOutput<TOutput> acquisition()
        { 
            static_assert(std::is_same<TAgent, TAlgorithm::agent_type>::value, "Algorithm and Flow do not manipulate same agent type");

            auto task       = taskflow.placeholder();

            Operation operation {task, Role::Acquisition};
            auto output = std::make_shared<TOutput>();
            
            task.work(
                [algos = this->algorithms, a = this->agent, res = output]() mutable
                {
                    *output = algos->get<TName, TAlgorithm>()(a);
                }
            );

            return { operation, output };
        };

        template<typename T, typename ... Args>
        OperationOutput<T> static_value(Args&& ... args)
        {
            auto task = taskflow.placeholder();
            Operation operation {task, Role::Value};

            auto output = std::make_shared<T>(T{ std::forward<Args>(args)... });

            // Empty task that is just mainting a counter for the shared ptr.
            task.work([res = (output)](){});

            return { operation, output };
        }

        // Execution
        // =========
        template<typename Callable>
        void run(tf::Executor& executor, Callable&& callable)
        {
            status = executor.run(taskflow, std::forward<Callable>(callable));
        };

        void cancel()
        {
            status.cancel();
        };

        void wait()
        {
            status.wait();
        };

    private:
        tf::Taskflow        taskflow    {};
        tf::Future<void>    status      {};

        TAgent& agent;
        Algorithms * const * const algorithms;
    };


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
}
