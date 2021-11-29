#ifndef DYNAMO_GUI_INFLUENCE_GRAPH_VIEWER_HPP
#define DYNAMO_GUI_INFLUENCE_GRAPH_VIEWER_HPP

#include <imgui-addons/nodes_editor.hpp>
#include <dynamo/strategies/influence_graph.hpp>

namespace dynamo::widgets
{
    template<typename T>
    class InfluenceGraphViewer : public ImGui::Flow::BipartiteGraph
    {
        using Behaviour_t   = Behaviour<std::vector<Influence<T>>, std::vector<T>>;

    public:
        InfluenceGraphViewer() = default;
        InfluenceGraphViewer(dynamo::InfluenceGraph<T>* graph, std::function<std::string(const T&)> get_name) 
            : graph{ graph }, name_of { get_name}
        {
            build();
        }

        void change(dynamo::InfluenceGraph<T>* new_graph)
        {
            graph = new_graph;
            highest.clear();
            clear();
            build();
        }

    private:
        void build()
        {
            std::unordered_map<const Behaviour_t *, ImGui::Flow::Node *> behaviour_imnodes{};
            for (const auto* behaviour : graph->behaviours())
            {
                auto& imnode = left_node(behaviour->name());
                imnode.output_pin("");
                behaviour_imnodes.emplace(behaviour, &imnode);
            }

            std::unordered_map<const T *, ImGui::Flow::Node *> object_imnodes{};
            for (const auto& object : graph->values())
            {
                auto& imnode = right_node(name_of(object));
                imnode.input_pin("");
                object_imnodes.emplace(&object, &imnode);
                size_t index = graph->index(object);
                if (graph->is_highest(index))
                    highest.emplace(&imnode);
                if (index == graph->result_index())
                    selected = &imnode;
            }

            for (const auto [behaviour_index, object_index] : graph->positive_influences())
            {
                positive_link(behaviour_imnodes[graph->behaviour(behaviour_index)], object_imnodes[&graph->value(object_index)]);
            }

            for (const auto [behaviour_index, object_index] : graph->negative_influences())
            {
                negative_link(behaviour_imnodes[graph->behaviour(behaviour_index)], object_imnodes[&graph->value(object_index)]);
            }
        }

        void render_graph() const override
        {
            ImVec2 origin(50.f, 50.f);
            ImVec2 cursor(origin);
            ImVec2 offset(200.f, 50.f);
            float max_length = 0.f;

            ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::BLUE_n);
            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::BLUE_h);
            ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::BLUE_s);
            for (const auto& node : left_nodes)
            {
                ImVec2 dimensions = node.render();
                ImNodes::SetNodeGridSpacePos(node.id, cursor);
                ImNodes::SetNodeDraggable(node.id, false);
                if (max_length < dimensions.x)
                    max_length = dimensions.x;
                cursor.y += dimensions.y + offset.y;
            }
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();

            cursor.y = origin.y;
            cursor.x += max_length + offset.x;

            for (const auto& node : right_nodes) {
                {// SET STYLE
                    if(&node == selected)
                    {
                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::GREEN_s);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::GREEN_h);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::GREEN_n);
                    }
                    else if (highest.contains(&node))
                    {
                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::GREEN_n);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::GREEN_h);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::GREEN_s);
                    }
                    else {
                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::ORANGE_n);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::ORANGE_h);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::ORANGE_s);
                    }
                }
                ImVec2 dimensions = node.render();

                    ImNodes::SetNodeGridSpacePos(node.id, cursor);
                    ImNodes::SetNodeDraggable(node.id, false);
                    if (max_length < dimensions.x)
                        max_length = dimensions.x;
                    cursor.y += dimensions.y + offset.y;
                ImNodes::PopColorStyle();
                ImNodes::PopColorStyle();
                ImNodes::PopColorStyle();
            }

            ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::GREEN_n);
            ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::GREEN_h);
            ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::GREEN_s);
            for (const auto& link : positive_links) {
                link.render();
            }
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();

            ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::RED_n);
            ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::RED_h);
            ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::RED_s);
            for (const auto& link : negative_links) {
                link.render();
            }
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }

    private:
        dynamo::InfluenceGraph<T>* graph {nullptr};
        std::unordered_set<const ImGui::Flow::Node *>  highest{};
        std::function<std::string(const T&)> name_of{ [](const T&) {return "Undefined"; } };
        ImGui::Flow::Node *  selected {nullptr};
    };
}

#endif //DYNAMO_GUI_INFLUENCE_GRAPH_VIEWER_HPP
