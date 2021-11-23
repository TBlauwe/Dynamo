#ifndef IMGUI_ADDONS_GRAPH_HPP
#define IMGUI_ADDONS_GRAPH_HPP

#include <imnodes.h>
#include <imgui-addons/imgui-addons.hpp>
#include <string>
#include <vector>
#include <unordered_set>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

namespace ImGui{

    namespace Graph {
        class GraphViewer {
            friend class Node;

        public:
            explicit GraphViewer(); 
            GraphViewer(const GraphViewer& that);
            GraphViewer& operator=(const GraphViewer& that);
            ~GraphViewer();                        

        public:
            void render() const;

        protected:
            inline size_t next_id() { return current_id++; }
        
        private:    
            virtual void _render() const = 0;

        private:
            size_t current_id { 0 };
            ImNodesEditorContext* editor { nullptr };
        };

        struct Pin {
            size_t id;
            const char * name;

            Pin(const size_t id, const char * name) :
                id{ id },
                name{ name }
            {}

            inline void render() const {
                ImGui::Text(name);
            }
        };

        struct Link {
            size_t id;
            size_t start;
            size_t end;

            Link(const size_t id, const Pin& start_pin, const Pin& end_pin) :
                id{ id },
                start{ start_pin.id },
                end{ end_pin.id }
            {}

            inline void render() const { 
                ImNodes::Link(id, start, end); 
            }
        };

        struct Node {

            GraphViewer*    graph;
            size_t          id;
            const char*     name;
            std::list<Pin>	input_pins{};
            std::list<Pin>	output_pins{};

            Node(GraphViewer* graph, const char * name) :
                graph  { graph },
                id     { graph->next_id() },
                name   { name }
            {}

            inline Pin& input_pin(const char * _name)
            {
                return input_pins.emplace_back(graph->next_id(), _name);
            }

            inline Pin& output_pin(const char * _name)
            {
                return output_pins.emplace_back(graph->next_id(), _name);
            }

            ImVec2 render() const
            {
                ImGui::PushID(id);
                ImNodes::BeginNode(id);
                {//TITLE BAR
                    ImNodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(name);
                    ImNodes::EndNodeTitleBar();
                }
                {//Input pin
                    auto input_pins_it = input_pins.begin();
                    auto output_pins_it = output_pins.begin();
                    for (int i = 0; i < std::max(input_pins.size(), output_pins.size()); i++)
                    {
                        bool input_pin_has_been_added = false;
                        if (input_pins_it != input_pins.end())
                        {
                            ImNodes::BeginInputAttribute(input_pins_it->id);
                            input_pins_it->render();
                            ImNodes::EndInputAttribute();
                            input_pin_has_been_added = true;
                            std::advance(input_pins_it, 1);
                        }

                        if (output_pins_it != output_pins.end())
                        {
                            if (input_pin_has_been_added)
                            {
                                ImGui::SameLine();
                                ImGui::Spacing();
                                ImGui::SameLine();
                            }
                                
                            ImNodes::BeginOutputAttribute(output_pins_it->id);
                            if (!input_pin_has_been_added)
                            {
                                ImGui::Spacing();
                                ImGui::Spacing();
                                ImGui::SameLine();
                            }
                            output_pins_it->render();
                            ImNodes::EndOutputAttribute();
                            std::advance(output_pins_it, 1);
                        }
                    }
                }
                ImNodes::EndNode();
                ImGui::PopID();
                return ImNodes::GetNodeDimensions(id);
            }
        };

    //    template<typename Tu, typename Tv>
    //    class ColoredBipartiteGraph : public Widgets::GraphViewer {
    //    protected:
    //        std::unordered_map<std::string, Node<Tu>> U {};
    //        std::unordered_map<std::string, Node<Tv>> V {};
    //        std::vector<Link<Tu, Tv>> positive_links {};
    //        std::vector<Link<Tu, Tv>> negative_links {};

    //        std::unordered_set<std::string> U_active {};
    //        std::unordered_set<std::string> V_highest {};

    //        const ImVec2 origin {ImVec2(50.f, 50.f )};
    //        const ImVec2 offset {ImVec2(200.f, 20.f )};

    //    public:

    //        explicit ColoredBipartiteGraph(const char * name) :
    //                GraphViewer(name)
    //        {}

    //        inline Node<Tu>& add_left_node(const char * name, const Tu* data, std::function<void(const Tu*)> render_body)
    //        {
    //            return (U.emplace(name, Node<Tu>{*this, name, data, render_body})).first->second;
    //        }

    //        inline Node<Tv>& add_right_node(const char * name, const Tv* data, std::function<void(const Tv*)> render_body)
    //        {
    //            return (V.emplace(name, Node<Tv>{*this, name, data, render_body})).first->second;
    //        }

    //        inline void link(Pin<Tu>& start_pin, Pin<Tv>& end_pin, bool positive) {
    //            if (positive) {
    //                positive_links.emplace_back(next_id(), start_pin, end_pin);
    //            }
    //            else {
    //                negative_links.emplace_back(next_id(), start_pin, end_pin);
    //            }
    //        }

    //        inline Node<Tu>& find_left_node_from(const char * name) {
    //            return U.find(name)->second;
    //        }

    //        inline Node<Tv>& find_right_node_from(const char * name) {
    //            return V.find(name)->second;
    //        }

    //        inline std::vector<const Link<Tu, Tv>&> get_in_edges_of(const Node<Tv>& node) {
    //            std::vector<const Link<Tu, Tv>&> edges;
    //            for (const Link<Tu, Tv>& link : positive_links) {
    //                if (link.end_pin.node.name == node.name)
    //                    edges.push_back(link);
    //            }

    //            for (const Link<Tu, Tv>& link : negative_links) {
    //                if (link.end_pin.node.name == node.name)
    //                    edges.push_back(link);
    //            }
    //            return edges;
    //        }

    //        inline std::vector<const Link<Tu, Tv>&> get_out_edges_of(const Node<Tu>& node) {
    //            std::vector<const Link<Tu, Tv>&> edges;
    //            for (const Link<Tu, Tv>& link : positive_links) {
    //                if (link.start_pin.node.name == node.name)
    //                    edges.push_back(link);
    //            }

    //            for (const Link<Tu, Tv>& link : negative_links) {
    //                if (link.start_pin.node.name == node.name)
    //                    edges.push_back(link);
    //            }
    //            return edges;
    //        }

    //        inline void refresh(const std::unordered_set<std::string>& elements) {
    //            U_active = std::unordered_set{ elements };
    //        }

    //        inline void compute_highest() {
    //            V_highest.clear();
    //            int max_value = 0;

    //            auto highest_elt = std::max_element(V.begin(), V.end(), [](const std::pair<std::string, Node<Tv>>& pair_a, const std::pair<std::string, Node<Tv>>& pair_b) {
    //                return pair_a.second.score < pair_b.second.score;
    //            });
    //            max_value = highest_elt->second.score;

    //            for (const auto& [name, node] : V) {
    //                if (node.score == max_value)
    //                    V_highest.insert(name);
    //            }
    //        }

    //        inline void clear_influence() {
    //            positive_links.clear();
    //            negative_links.clear();
    //            V_highest.clear();
    //            for (auto& [name, node] : V) {
    //                node.score = 0;
    //            }
    //        }

    //        inline void clear() {
    //            U.clear();
    //            V.clear();
    //            positive_links.clear();
    //            negative_links.clear();
    //            U_active.clear();
    //            V_highest.clear();
    //        }

    //    private:

    //        void _render() const override {

    //            ImVec2 cursor(origin);
    //            ImVec2 dimensions;
    //            float max_length = 0.f;

    //            for (const auto& [name, u] : U) {
    //                {// SET STYLE
    //                    if (U_active.contains(name)) {
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::GRAY_n);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::GRAY_h);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::GRAY_s);
    //                    }
    //                    else {
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::BLUE_n);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::BLUE_h);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::BLUE_s);
    //                    }
    //                }

    //                dimensions = u.render();

    //                { // SET POSITION
    //                    ImNodes::SetNodeGridSpacePos(u.id, cursor);
    //                    ImNodes::SetNodeDraggable(u.id, false);
    //                    if (max_length < dimensions.x)
    //                        max_length = dimensions.x;
    //                    cursor.y += dimensions.y + offset.y;
    //                }
    //                ImNodes::PopColorStyle();
    //                ImNodes::PopColorStyle();
    //                ImNodes::PopColorStyle();
    //            }
    //            cursor.y = origin.y;
    //            cursor.x += max_length + offset.x;

    //            for (const auto& [name, v] : V) {
    //                {// SET STYLE
    //                    if (V_highest.contains(name)) {
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::GREEN_n);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::GREEN_h);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::GREEN_s);
    //                    }
    //                    else {
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::ORANGE_n);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::ORANGE_h);
    //                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::ORANGE_s);
    //                    }
    //                }
    //                dimensions = v.render();

    //                { // SET POSITION
    //                    ImNodes::SetNodeGridSpacePos(v.id, cursor);
    //                    ImNodes::SetNodeDraggable(v.id, false);
    //                    if (max_length < dimensions.x)
    //                        max_length = dimensions.x;
    //                    cursor.y += dimensions.y + offset.y;
    //                }
    //                ImNodes::PopColorStyle();
    //                ImNodes::PopColorStyle();
    //                ImNodes::PopColorStyle();
    //            }

    //            ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::GREEN_n);
    //            ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::GREEN_h);
    //            ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::GREEN_s);
    //            for (const Link<Tu, Tv>& link : positive_links) {
    //                link.render();
    //            }

    //            ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::RED_n);
    //            ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::RED_h);
    //            ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::RED_s);
    //            for (const Link<Tu, Tv>& link : negative_links) {
    //                link.render();
    //            }
    //        };
    //    };
    //}

        class FlowGraph : public GraphViewer
        {
        public:
            explicit FlowGraph() : GraphViewer(){}

            inline Node& node(const char * name)
            {
                return nodes.emplace_back(this, name);
            }

            inline void link(Node* a, const char * output_name, Node* b, const char* input_name)
            {
                links.emplace_back(next_id(), a->output_pin(output_name), b->input_pin(input_name));
            }

            inline void clear()
            {
                nodes.clear();
                links.clear();           
            }

        private:
            void _render() const override 
            {

                const ImVec2 origin (50.f, 50.f );
                const ImVec2 offset (200.f, 20.f );

                ImVec2 cursor(origin);
                ImVec2 dimensions (0.0f, 0.0f);
                float max_length = 0.f;

                for (const auto& node : nodes) 
                {
                    { // SET STYLE
                        ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::BLUE_n);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::BLUE_h);
                        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::BLUE_s);
                    }

                    dimensions = node.render();

                    { // SET POSITION
                        ImNodes::SetNodeGridSpacePos(node.id, cursor);
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
            };

        protected:
            std::list<Node> nodes{};
            std::vector<Link> links{};
        };
    }
}
#endif //IMGUI_ADDONS_GRAPH_HPP
