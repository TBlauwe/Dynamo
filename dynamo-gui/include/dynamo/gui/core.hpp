#ifndef DYNAMO_GUI_CORE_HPP
#define DYNAMO_GUI_CORE_HPP

#include <mutex>
#include <string>

#include <taskflow/taskflow.hpp>

#include <dynamo/gui/widgets/brain_viewer.hpp>
#include <dynamo/gui/widgets/influence_graph_viewer.hpp>

namespace dynamo
{
    class TasksObs : public tf::ObserverInterface {
    public:
        TasksObs() = default;

        inline bool contains(tf::Task task) const
        {
            return container.contains(task.hash_value());
        }

        inline bool contains(size_t hash) const
        {
            return container.contains(hash);
        }

        inline void set_up(size_t num_workers) override final {}

        inline void on_entry(tf::WorkerView w, tf::TaskView tv) override final {
            push(tv.hash_value());
        }

        inline void on_exit(tf::WorkerView w, tf::TaskView tv) override final {
            remove(tv.hash_value());
        }

    private:
        inline void push(size_t hash_value)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            container.emplace(hash_value);
        }

        inline void remove(size_t hash_value)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            container.erase(hash_value);
        }

    private:
        std::unordered_set<size_t> container {};
        mutable std::mutex mutex_;
    };

    namespace type
    {
        struct GUI {
            bool show_widget = false;
        };

        struct BrainViewer
        {
            widgets::BrainViewer viewer;

            BrainViewer() : viewer() {};
            BrainViewer(flecs::entity e, tf::Taskflow* taskflow, const type::ProcessDetails* details) : viewer(e, taskflow, details) {};
        };

        template<typename T>
        struct InfluenceGraphViewer
        {
            widgets::InfluenceGraphViewer<T> viewer {};

            InfluenceGraphViewer() = default;
            InfluenceGraphViewer(dynamo::InfluenceGraph<T>* graph, std::function<std::string(const T&)> name_of) : viewer(graph, name_of) {};
        };

        struct ActiveTasks
        {
            std::shared_ptr<TasksObs> observer;
        };
    }
}

#endif //DYNAMO_GUI_CORE_HPP
