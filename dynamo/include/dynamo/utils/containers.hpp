#ifndef DYNAMO_CONTAINERS_H
#define DYNAMO_CONTAINERS_H

#include <any>
#include <typeindex>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <optional>

/**
@file dynamo/utils/containers.hpp
@brief Defines some containers used by the library 
*/

/**
@class TypeMap

@brief Defines an associative container of type / value.

Usage :
@code{.cpp}
TypeMap map;
auto elt = map.add<MyType>(); // Add and return an element by type.
auto elt = map.get<MyType>(); // Get element by type.
@endcode
*/
class TypeMap
{

public:
    /**
    @brief Get const element by type @c T.
    */
    template<class T>
    const T& get() const
    {
        return std::any_cast<const T&>(container.at(typeid(T)));
    }

    /**
    @brief Get mutable element by type @c T.
    */
    template<class T>
    T& get_mut()
    {
        return std::any_cast<T&>(container.at(typeid(T)));
    }

    /**
    @brief Add a new element @c T. Only one element of a specific type can be added.

    @tparam Must be @c DefaultConstructible.
    */
    template<class T>
    T& add()
    {
        if (container.contains(typeid(T)))
            return std::any_cast<T&>(container[typeid(T)]);
        else
            return std::any_cast<T&>(container[typeid(T)] = T());
    }

private:
    std::unordered_map<std::type_index, std::any> container;
};

/**
@class ThreadSafeQueue

@brief Credits : https://codetrips.com/2020/07/26/modern-c-writing-a-thread-safe-queue/
*/
template<typename T>
class ThreadsafeQueue {
      std::queue<T> queue_;
  mutable std::mutex mutex_;

  // Moved out of public interface to prevent races between this
  // and pop().
  [[nodiscard]] bool empty() const {
    return queue_.empty();
  }

 public:
  ThreadsafeQueue() = default;
  ThreadsafeQueue(const ThreadsafeQueue<T> &) = delete ;
  ThreadsafeQueue& operator=(const ThreadsafeQueue<T> &) = delete ;

  ThreadsafeQueue(ThreadsafeQueue<T>&& other) noexcept(false) {
    std::lock_guard<std::mutex> lock(mutex_);
    //static_assert(!empty(), "Moving into a non-empty queue");
    queue_ = std::move(other.queue_);
  }

  virtual ~ThreadsafeQueue() noexcept(false) {
    std::lock_guard<std::mutex> lock(mutex_);
    //static_assert(!empty(), "Destroying into a non-empty queue");
  }

  [[nodiscard]] unsigned long size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

  std::optional<T> pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return {};
    }
    T tmp = queue_.front();
    queue_.pop();
    return tmp;
  }

  void push(const T &item) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(item);
  }
};

#endif DYNAMO_TYPE_MAP_H
