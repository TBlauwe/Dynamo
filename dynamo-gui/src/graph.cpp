#include <dynamo/gui/widgets/graph.hpp>
#include <dynamo/gui/core.hpp>
#include <string>

void dynamo::widgets::BrainViewer::compute(tf::Taskflow* taskflow) {
    if(taskflow == nullptr){
        return;
    }

    if(taskflow != last_taskflow){
        compute_graph(taskflow);
    }
}

void dynamo::widgets::BrainViewer::_render() const
{
    const ImVec2 origin(50.f, 50.f);
    const ImVec2 offset(200.f, 20.f);

    ImVec2 cursor(origin);
    ImVec2 dimensions(0.0f, 0.0f);
    float max_length = 0.f;

    auto& active_tasks = *entity.world().get<type::ActiveTasks>()->observer;
    for (const auto& node : nodes)
    {
        { // SET STYLE
            if (active_tasks.contains(nodes_hash.at(&node)))
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::GREEN_n);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::GREEN_h);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::GREEN_s);
            }
            else
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::ORANGE_n);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::ORANGE_h);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::ORANGE_s);
            }
        }

        dimensions = node.render();

        { // SET POSITION
            ImNodes::SetNodeGridSpacePos(node.id, cursor);
            ImNodes::SetNodeDraggable(node.id, false);
            if (max_length < dimensions.x)
                max_length = dimensions.x;
            cursor.y += dimensions.y + offset.y;
        }

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }

    ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::GREEN_n);
    ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::GREEN_h);
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::GREEN_s);
    for (const auto& link : links)
    {
        link.render();
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

//void dynamo::widgets::BrainViewer::_render() const {
//    ogdf::GraphAttributes GA(graph);
//
//    // Layout
//    for (ogdf::node v : graph.nodes)
//        GA.width(v) = GA.height(v) = 20.0;
//
//    ogdf::SugiyamaLayout SL;
//    SL.setRanking(new ogdf::OptimalRanking);
//    SL.setCrossMin(new ogdf::MedianHeuristic);
//
//    auto *ohl = new ogdf::OptimalHierarchyLayout;
//    ohl->layerDistance(30.0);
//    ohl->nodeDistance(25.0);
//    ohl->weightBalancing(0.8);
//    SL.setLayout(ohl);
//
//    SL.call(GA);
//
//    //for (const auto& node : nodes) {
//        //node.render();
//        //ImNodes::SetNodeGridSpacePos(static_cast<int>(node.id), {static_cast<float>(GA.x(ogdf_node)), static_cast<float>(GA.y(ogdf_node))});
//        //ImNodes::SetNodeDraggable(static_cast<int>(node.id), false);
//    //}
//}

void dynamo::widgets::BrainViewer::compute_graph(tf::Taskflow* taskflow) {
    clear();
    nodes_hash.clear();

    last_taskflow = taskflow;

    taskflow->for_each_task([this](const tf::Task& task){
        switch (task.type()) {
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
            std::cout << "Task name : " << task.name() << " | Hash : " << task.hash_value() << "\n";
            nodes_hash.emplace(&node(task.name().c_str()), task.hash_value());
            //nodes.emplace_back(ImGui::Widgets::Node<tf::Task>{*this, task.name().c_str(), &task, [](const tf::Task * task){}});
            break;
        }
    });
}