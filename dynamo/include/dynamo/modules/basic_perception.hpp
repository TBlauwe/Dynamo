#ifndef DYNAMO_BASIC_PERCEPTION_HPP
#define DYNAMO_BASIC_PERCEPTION_HPP

#endif //DYNAMO_BASIC_PERCEPTION_HPP
namespace dynamo{
    enum class BasicPerception{
        HEARING,
        SMELL,
        TASTE,
        TOUCH,
        VISION,
        GLOBAL
    };

    namespace module{

        struct GlobalPerception{
            explicit GlobalPerception(flecs::world& world){
                world.module<GlobalPerception>();
                world.import<module::Core>();
            }
        };
    }
}
