#ifndef DYNAMO_TYPE_MAP_H
#define DYNAMO_TYPE_MAP_H

#include <any>
#include <typeindex>
#include <unordered_map>

/**
@file dynamo/utils/type_map.hpp
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
    @brief Get element by type @c.
    */
    template<class T>
    T& get()
    {
        return std::any_cast<T&>(container.at(typeid(T)));
    }

    /**
    @brief Add a new element @c T. Only one element of a specific type can be added.
    Multiple calls of the same type will result in undefined behaviour.

    @tparam Must be @c DefaultConstructible.
    */
    template<class T>
    T& add()
    {
        return std::any_cast<T&>(container[typeid(T)] = T());
    }

private:
    std::unordered_map<std::type_index, std::any> container;
};

#endif DYNAMO_TYPE_MAP_H
