#include <string>

#include <dynamo/gui/widgets/brain_viewer.hpp>
#include <dynamo/gui/core.hpp>

dynamo::widgets::BrainViewer::BrainViewer(flecs::entity e, tf::Taskflow* taskflow) :
    entity{ e }, taskflow{ taskflow }, ImGui::Flow::Graph()
{
    build();
}

void dynamo::widgets::BrainViewer::render_graph() const
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
                ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::BLUE_n);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::BLUE_h);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::BLUE_s);
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

    ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::WHITE_n);
    ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::WHITE_h);
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::WHITE_s);
    for (const auto& link : links)
    {
        link.render();
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

void dynamo::widgets::BrainViewer::build() {
    ogdf::Graph g;
    ogdf::GraphAttributes ga(g);
    std::unordered_map<ImGui::Flow::Node const*, ogdf::node> matching{};

    taskflow->for_each_task(
        [this, &g, &matching](const tf::Task& task)
        {
            auto& n = node(task.name().c_str());
            size_t hash = task.hash_value();
            nodes_hash.emplace(&n,hash);
            hash_nodes.emplace(hash, &n);
            matching.emplace(&n, g.newNode());
        }
    );

    taskflow->for_each_task(
        [this, &g, &matching](tf::Task task) mutable
        {
            auto n = hash_nodes.at(task.hash_value());
            auto no = matching.at(n);
            task.for_each_dependent(
                [&n, this, &matching, &no , &g] (tf::Task child) mutable
                {
                    auto m = hash_nodes.at(child.hash_value());
                    g.newEdge(matching.at(m), no);
                    link(const_cast<ImGui::Flow::Node*>(m), "Output", const_cast<ImGui::Flow::Node*>(n), "Input");
                }
            );
        }
    );

    // Layout
    for (auto v : g.nodes)
    {
        ga.width(v) = 100.0f;
        ga.height(v) = 100.0f;
    }

    ogdf::SugiyamaLayout SL;
    SL.setRanking(new ogdf::OptimalRanking);
    SL.setCrossMin(new ogdf::MedianHeuristic);

    auto* ohl = new ogdf::OptimalHierarchyLayout;
    ohl->layerDistance(200.0);
    ohl->nodeDistance(200.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);
    SL.call(ga);

    for (const auto& node : nodes)
    {
        auto v = matching.at(&node);
        nodes_pos.emplace(&node, ImVec2{ static_cast<float>(ga.y(v)), static_cast<float>(ga.x(v)) });
    }
}