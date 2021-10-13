#ifndef DYNAMO_RELATIONS_HPP
#define DYNAMO_RELATIONS_HPP

/**
 * For more information, see https://flecs.docsforge.com/master/relations-manual/.
 */
namespace dynamo::relation{
    /**
     * Relation from an entity A to a percept B : "A perceive B"
     */
    struct perceive{};

    /**
     * Relation from an entity A to an entity B : "A source B" => "A is coming from B".
     * Used notably to store the source of a percept.
     */
    struct source{};

    /**
     * Relation from an entity A to an organisation B : "A belongs_to B"
     */
    struct belongs_to{};
}

#endif //DYNAMO_RELATIONS_HPP
