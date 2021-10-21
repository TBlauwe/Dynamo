#include <dynamo/gui/widgets/graph.hpp>
#include <string>

void dynamo_gui::widget::BrainViewer::compute(tf::Taskflow* taskflow) {
    if(taskflow == nullptr){
        return;
    }

    if(taskflow == last_taskflow){
        compute_graph(taskflow);
    }
}

void dynamo_gui::widget::BrainViewer::_render() const {
    ogdf::GraphAttributes GA(graph);

    // Layout
    for (ogdf::node v : graph.nodes)
        GA.width(v) = GA.height(v) = 20.0;

    ogdf::SugiyamaLayout SL;
    SL.setRanking(new ogdf::OptimalRanking);
    SL.setCrossMin(new ogdf::MedianHeuristic);

    auto *ohl = new ogdf::OptimalHierarchyLayout;
    ohl->layerDistance(30.0);
    ohl->nodeDistance(25.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);

    SL.call(GA);

    //for (const auto& node : nodes) {
        //node.render();
        //ImNodes::SetNodeGridSpacePos(static_cast<int>(node.id), {static_cast<float>(GA.x(ogdf_node)), static_cast<float>(GA.y(ogdf_node))});
        //ImNodes::SetNodeDraggable(static_cast<int>(node.id), false);
    //}
}

void dynamo_gui::widget::BrainViewer::compute_graph(tf::Taskflow* taskflow) {
    last_taskflow = taskflow;
    graph.clear();
    //nodes.clear();
    //links.clear();

    taskflow->for_each_task([this](const tf::Task& task){
        switch(task.type()){
            case tf::TaskType::MODULE:
            case tf::TaskType::PLACEHOLDER:
            case tf::TaskType::CUDAFLOW:
            case tf::TaskType::SYCLFLOW:
            case tf::TaskType::STATIC:
            case tf::TaskType::DYNAMIC:
            case tf::TaskType::CONDITION:
            case tf::TaskType::ASYNC:
            case tf::TaskType::UNDEFINED:
            default:
                //nodes.emplace(graph.newNode(), ImGui::Widgets::Node<std::string>{*this, task.name().c_str(), task, [](const tf::Task * task){}});
                //nodes.emplace_back(ImGui::Widgets::Node<tf::Task>{*this, task.name().c_str(), &task, [](const tf::Task * task){}});
                break;
        }
    });
}