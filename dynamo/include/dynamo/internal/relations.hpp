#ifndef DYNAMO_RELATIONS_HPP
#define DYNAMO_RELATIONS_HPP

/**
@file dynamo/internal/relations.hpp
@brief Defines some basic relations.

For more information, see https://flecs.docsforge.com/master/relations-manual/.
*/
namespace dynamo::relation{
    /**
    @brief A perceive B.

    Relation from an entity A to a percept B.
    */
    struct perceive{};

    /**
    @brief A source B.

    Relation from an entity A to an entity B, meaning that "A is coming from B".
    Used notably to store the source of a percept.
    */
    struct source{};

    /**
    @brief A belongs_to B.

    Relation from an entity A to an organisation B, meaning that "A belongs_to B".
    */
    struct belongs_to{};
}

#endif //DYNAMO_RELATIONS_HPP
