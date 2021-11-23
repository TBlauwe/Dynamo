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

        ImVec2 dimensions = node.render();


        { // SET POSITION

            
            ImNodes::SetNodeGridSpacePos(node.id, nodes_pos.at(&node));
            ImNodes::SetNodeDraggable(node.id, false);
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

void dynamo::widgets::BrainViewer::compute_graph(tf::Taskflow* taskflow) {
    clear();
    nodes_hash.clear();
    hash_nodes.clear();
    nodes_pos.clear();
    last_taskflow = taskflow;

    ogdf::Graph g;
    ogdf::GraphAttributes ga(g);
    std::unordered_map<ImGui::Graph::Node const*, ogdf::node> matching{};


    taskflow->for_each_task([this, &g, &matching](const tf::Task& task){
            auto& n = node(task.name().c_str());
            size_t hash = task.hash_value();
            nodes_hash.emplace(&n,hash);
            hash_nodes.emplace(hash, &n);
            matching.emplace(&n, g.newNode());
    });

    taskflow->for_each_task([this, &g, &matching](const tf::Task& task) {
        auto& n = *hash_nodes.at(task.hash_value());
        task.for_each_dependent([&n, this](tf::Task child) {
            auto& m = *hash_nodes.at(child.hash_value());
            link(m, "Output", n, "Input");
            });
        });


    // Layout
    for (auto v : g.nodes)
    {
        ga.width(v) = 200;
        ga.height(v) = 50.0f;
    }

    ogdf::SugiyamaLayout SL;
    SL.setRanking(new ogdf::OptimalRanking);
    SL.setCrossMin(new ogdf::MedianHeuristic);

    auto* ohl = new ogdf::OptimalHierarchyLayout;
    ohl->layerDistance(30.0);
    ohl->nodeDistance(25.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);

    SL.call(ga);

    for (const auto& node : nodes)
    {
        auto v = matching.at(&node);
        nodes_pos.emplace(&node, ImVec2{ static_cast<float>(ga.x(v)), static_cast<float>(ga.y(v)) });
    }
}