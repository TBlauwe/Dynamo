#ifndef DYNAMO_CORE_HPP
#define DYNAMO_CORE_HPP

#include <dynamo/gui/widgets/graph.hpp>
#include <string>

namespace dynamo::type {
    struct GUI {
        bool show_widget = false;
    };

    struct BrainViewer
    {
        widgets::BrainViewer viewer{};
    };
}

#endif //DYNAMO_CORE_HPP
