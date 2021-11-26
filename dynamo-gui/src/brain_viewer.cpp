#include <string>

#include <dynamo/gui/widgets/brain_viewer.hpp>
#include <dynamo/gui/core.hpp>

dynamo::widgets::BrainViewer::BrainViewer(flecs::entity e, const tf::Taskflow* taskflow, const type::ProcessDetails* details) :
    entity{ e }, taskflow{ taskflow }, _details{ details }, ImGui::Flow::Graph()
{
    build();
}

const ImGui::Flow::Node * dynamo::widgets::BrainViewer::find_node(int i) const
{
    auto it = std::find_if(nodes.begin(), nodes.end(), [i](const ImGui::Flow::Node& node) {return node.id == i; });
    if (it == nodes.end())
        return nullptr;
    else
        return &(*it);
}

size_t dynamo::widgets::BrainViewer::find_task(const ImGui::Flow::Node * const node) const
{
    return imnodes_hash.at(node);
}

void dynamo::widgets::BrainViewer::render_graph() const
{
    auto& active_tasks = *entity.world().get<type::ActiveTasks>()->observer;

    for (const auto& node : nodes)
    {
        auto hash = imnodes_hash.at(&node);
        //auto target_details = _details->find(hash);
        { // SET STYLE
            if (!entity.has<type::IsProcessing>())
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::ORANGE_n);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::ORANGE_h);
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::ORANGE_s);
            }
            else
            {
                if (active_tasks.contains(hash))
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

        }

        ImVec2 dimensions = node.render();

        { // SET POSITION

            ImNodes::SetNodeGridSpacePos(node.id, imnodes_pos.at(&node));
            ImNodes::SetNodeDraggable(node.id, false);
        }

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }

    ImNodes::PushColorStyle(ImNodesCol_Link,            ImGui::Color::WHITE_n);
    ImNodes::PushColorStyle(ImNodesCol_LinkHovered,     ImGui::Color::WHITE_h);
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected,    ImGui::Color::WHITE_s);
    for (const auto& link : links)
    {
        link.render();
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

void dynamo::widgets::BrainViewer::build() {
    ogdf::Graph             layout_graph;
    ogdf::GraphAttributes   graph_attributes(layout_graph);

    std::unordered_map<ImGui::Flow::Node const*, ogdf::node> imnodes_ogdf_nodes{};

    // -- First, initialize a node for each task
    taskflow->for_each_task(
        [this, &layout_graph, &imnodes_ogdf_nodes](const tf::Task& task)
        {
            auto&   imnode  = node(task.name().c_str());
            size_t  hash    = task.hash_value();

            imnodes_hash.emplace(&imnode, hash);
            hash_imnodes.emplace(hash, &imnode);

            imnodes_ogdf_nodes.emplace(&imnode, layout_graph.newNode());

        }
    );

    // -- Then, link them
    taskflow->for_each_task(
        [this, &layout_graph, &imnodes_ogdf_nodes](tf::Task task) mutable
        {
            auto hash           = task.hash_value();
            auto im_target      = hash_imnodes.at(hash);
            auto target         = imnodes_ogdf_nodes.at(im_target);
            auto target_details = _details->find(hash);

            task.for_each_dependent(
                [this, &imnodes_ogdf_nodes, &target , im_target, &layout_graph, &target_details] (tf::Task child) mutable
                {
                    auto hash = child.hash_value();
                    auto source = hash_imnodes.at(hash);
                    layout_graph.newEdge(imnodes_ogdf_nodes.at(source), target);

                    ImGui::Flow::Pin* source_flow_pin = nullptr;
                    if (source->output_pins.size() == 0)
                    {
                        source_flow_pin = &source->output_pin("");
                    }
                    else
                    {
                        source_flow_pin = &source->output_pins.front();
                    }

                    if (target_details.has_input_from(hash))
                    {
                        auto target_input_pin = &im_target->input_pin(target_details.input_name(hash));
                        link(source_flow_pin, target_input_pin);
                    }
                    else
                    {
                        auto target_input_pin = &im_target->input_pin("");
                        link(source_flow_pin, target_input_pin);
                    }

                }
            );
        }
    );

    // Layout
    for (auto v : layout_graph.nodes)
    {
        graph_attributes.width(v) = 100.0f;
        graph_attributes.height(v) = 100.0f;
    }

    ogdf::SugiyamaLayout SL;
    SL.setRanking(new ogdf::OptimalRanking);
    SL.setCrossMin(new ogdf::MedianHeuristic);

    auto* ohl = new ogdf::OptimalHierarchyLayout;
    ohl->layerDistance(200.0);
    ohl->nodeDistance(200.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);
    SL.call(graph_attributes);

    for (const auto& node : nodes)
    {
        auto v = imnodes_ogdf_nodes.at(&node);
        imnodes_pos.emplace(&node, ImVec2{ static_cast<float>(graph_attributes.y(v)), static_cast<float>(graph_attributes.x(v)) });
    }
}