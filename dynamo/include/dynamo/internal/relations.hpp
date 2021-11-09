#ifndef DYNAMO_RELATIONS_HPP
#define DYNAMO_RELATIONS_HPP

/**
@file dynamo/internal/relations.hpp
@brief Defines some basic relations.

By convention :
    - components and tags names are written using PascalCase.
    - relation names are written using snake_case.

For more information, see https://flecs.docsforge.com/master/relations-manual/.
*/
namespace dynamo::type {

    /**
    @brief Relation from an entity A to a percept B, meaning that "A perceives B".
    */
    struct perceive {};

    /**
    @brief Relation from an entity A to an entity B, meaning that "A is coming from B".

    Used notably to store the source of a percept.
    */
    struct source {};

    /**
    @brief Relation from an entity A to an organisation B, meaning that "A belongs_to B".
    */
    struct belongs_to {};

}  // namespace dynamo::type

#endif  // DYNAMO_RELATIONS_HPP
