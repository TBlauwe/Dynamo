#ifndef DYNAMO_TYPES_HPP
#define DYNAMO_TYPES_HPP

#include <dynamo/modules/core.hpp>

/**
 * Builder class to help create/manipulate percepts
 */
class PerceptBuilder{
    flecs::entity entity;

public:
    explicit PerceptBuilder(flecs::entity entity) : entity{entity}
    {
        assert(entity.has<dynamo::type::Percept>());
    };

    PerceptBuilder& perceived_by(flecs::entity e){
        e.add<dynamo::relation::perceive>(entity);
        return *this;
    }

    flecs::entity get(){ return entity; }
};


#endif //DYNAMO_TYPES_HPP
