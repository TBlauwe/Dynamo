#ifndef DYNAMO_CORE_H
#define DYNAMO_CORE_H

#include <flecs.h>

namespace dynamo{
    namespace tag{
        struct Agent{};
        struct Artefact{};
    }

    namespace component{
        struct Event{
            std::string name;
        };
    }
}

#endif //DYNAMO_CORE_H
