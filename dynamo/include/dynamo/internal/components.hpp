#pragma once

#include <vector>

#include <taskflow/taskflow.hpp>

#include <dynamo/utils/containers.hpp>

/**
@file dynamo/internal/components.hpp
@brief Defines some basic components/tags

By convention :
    - components and tags names are written using PascalCase.
    - relation names are written using snake_case.
*/

/**
@brief Contains structs and classes used as a type for flecs.

By convention :
    - components and tags names are written using PascalCase.
    - relation names are written using snake_case.
*/
namespace dynamo
{

    /**
     @brief Tag removed at the end of the current frame.
     /!\ Handle this tag before @c flecs::PostFrame /!\ .
     */
    struct CurrentFrame {};

    /**
    @brief When @c ttl (in seconds) reaches 0, the entity holding this component is destroyed.
    */
    struct Decay
    {
        /**
         Amount of time left to live in seconds.
         */
        float ttl;
    };

    /**
     @brief When @c remaining_time (in seconds) reaches 0, the relation for this cooldown
    is removed. Must be used as a relation with something else.

     @code{.cpp}
     entity.set<component::Cooldown, component::Attack>({1.0f}); // Relation is destroyed in 1 seconds.
     @endcode

     After cooldown is finished, the relation is removed allowing you to trigger something.

     @code{.cpp}

     world.system<component::Attack>("System_Attack")
                                    .term<component::Cooldown>().object<component::Attack>().oper(flecs::Not)
                                    .arg(1).object(flecs::Wildcard)
                                    .iter([](flecs::iter& iter, component::Attack* attack) {
                                        for(auto i : iter){
                                            auto e = iter.entity(i);
                                            // do something ....
                                            // and reset the cooldown
                                            e.set<component::Cooldown, component::Attack>({attack[i].cooldown});
                                         }
                                    });
    @endcode
    */
    struct Cooldown
    {
        /**
        Amount of time remaining before cooldown is finished (in seconds).
        */
        float remaining_time;
    };

    /**
    @brief Store read-only entities handle. Call @c mut(...), if you want to modify it.
    */
    struct Targets 
    {
        /**
        @brief A vector of read_only entities. Call @c mut(...), if you need to modify it.
        */
        std::vector<flecs::entity_view> entities;
    };

    /**
    @brief Little hack to delay the creation of @c Flow as it isn't copyable.
    */
    template<typename T>
    struct AddFlow 
    {
        bool    is_cyclic   { true };
        float   period      { 1.0f };
    };

    /**
    @brief Contains a flow
    */
    struct Flow 
    {
        /**
        @brief Taskflow.
        */
        tf::Taskflow taskflow;
    };

    /**
    @brief Explicit.
    */
    struct Counter 
    {
        size_t value {0};
    };

    /**
    @brief Holds a duration in milliseconds
    */
    struct Duration 
    {
        std::chrono::duration<double, std::milli> value {0};
    };

    /**
    @brief Holds a timestamp, initialized with current system clock when added.
    */
    struct Timestamp
    {
        std::chrono::system_clock::time_point value {std::chrono::system_clock::now()};
    };

    using CommandsQueue = ThreadsafeQueue<std::function<void(flecs::world&)>>;

    /**
    @brief Holds a pointer to the command queue to delay commands set during async tasks.
    */
    struct CommandsQueueHandle
    {
        CommandsQueue* queue {};
    };

    /**
    @brief Holds running status of the associated process.
    */
    struct Status 
    {
        /**
        @brief Current status of a running taskflow.
        */
        tf::Future<void> value;
        
        /**
        @brief Returns @c true or @c false if the process is finished or not.
        */
        const bool is_finished()
        {
            return !static_cast<bool>(value.wait_for(std::chrono::seconds(0)));
        }

        void cancel()
        {
            value.cancel();
        }
    };
}  // namespace dynamo
