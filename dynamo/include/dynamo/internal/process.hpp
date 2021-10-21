#ifndef DYNAMO_PROCESS_HPP
#define DYNAMO_PROCESS_HPP

#include <taskflow/taskflow.hpp>
#include <dynamo/internal/types.hpp>

namespace dynamo{
    template<typename TDataIn, typename TDataOut>
    class Behaviour{
        std::string m_name;
        std::function<bool(Agent)> activation_function;
        std::function<TDataOut(Agent, TDataIn)> computation_function;

    public:
        explicit Behaviour(const char * name) : m_name{name}{}
        bool is_active(Agent agent) {return activation_function(agent);};
        bool is_active(Agent agent, TDataIn data) {return computation_function(agent, data);};
        std::string name() { return m_name; }
    };

    template<typename TBehaviourOut, typename TProcessDataIn, typename TProcessDataOut>
    class Strategy {
    protected:
        std::vector<Behaviour<TProcessDataIn, TBehaviourOut>> behaviours {};

    public:
        void add_behaviour(Behaviour<TProcessDataIn, TBehaviourOut> behaviour){ behaviours.push_back(behaviour); }

        virtual TProcessDataOut run(Agent& agent, TProcessDataIn data) = 0;
    };

    class Process {
        tf::Executor& executor;
        tf::Taskflow taskflow;

    public:
        explicit Process(tf::Executor& executor) : executor{executor} ,taskflow{}{}
    };
}



#endif //DYNAMO_PROCESS_HPP
