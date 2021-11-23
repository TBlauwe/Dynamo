#ifndef DYNAMO_CORE_HPP
#define DYNAMO_CORE_HPP

#include <dynamo/gui/widgets/graph.hpp>
#include <taskflow/taskflow.hpp>
#include <mutex>
#include <string>

namespace dynamo
{
    class TasksObs : public tf::ObserverInterface {
    public:
        TasksObs() = default;

        inline bool contains(size_t hash_value) const
        {
            return container.contains(hash_value);
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

            BrainViewer() : viewer(0) {};
            BrainViewer(flecs::entity e) : viewer(e) {};
        };

        struct ActiveTasks
        {
            std::shared_ptr<TasksObs> observer;
        };
    }
}

#endif //DYNAMO_CORE_HPP
