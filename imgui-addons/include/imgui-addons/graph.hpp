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
    namespace Widgets {
        class GraphViewer {
        private:
            const char *    name;
            unsigned int    current_id{0};
            ImNodesEditorContext *editor{nullptr};

        public:
            explicit GraphViewer(const char *name);
            ~GraphViewer();

        public:
            void render() const;
            inline unsigned int next_id() { return current_id++; }

        protected:
            virtual void _render() const = 0;
        };
    }

    namespace Flow{
        template<typename T>
        struct Node;

        template<typename T>
        struct Pin{
            const size_t    id;
            const char*     name;
            Node<T>&        node;

            const std::function<void(Pin<T>&)> render_func;

            Pin(const size_t id, const char* name, Node<T>& node, const std::function<void(Pin<T>&)> render_func) :
                    id {id},
                    name {name},
                    node {node},
                    render_func {render_func}
            {}

            inline void render() const {
                ImNodes::BeginInputAttribute(id);
                {
                    render_func(this);
                }
                ImNodes::EndInputAttribute();
            }
        };

        template <typename Tu, typename Tv>
        struct Link{
            const size_t    id;
            const Pin<Tu>&  start_pin;
            const Pin<Tv>&  end_pin;

            Link(const size_t id, const Pin<Tu>& start_pin, const Pin<Tv>& end_pin) :
                    id {id},
                    start_pin {start_pin},
                    end_pin {end_pin}
            {}

            inline void render() const { ImNodes::Link(id, start_pin.id, end_pin.id); }
        };

        /**
         * Template class to represent a node holding a pointer to a T object.
         * @tparam T type of the holding data
         */
        template<typename T>
        struct Node {
            Widgets::GraphViewer&   graph;
            const T*			    data;
            const size_t		    id;
            const char *	        name;

            std::unordered_map<std::string, Pin<T>>		input_pins {};
            std::unordered_map<std::string, Pin<T>>		output_pins {};
            std::function<void(const T*)>				render_body;

            Node(Widgets::GraphViewer& graph, const char * name, const T* data, std::function<void(const T*)> render_body) :
                    id {graph.next_id()},
                    graph {graph},
                    name {name},
                    data {data},
                    render_body {render_body}
            {}

            inline Pin<T> add_input_pin(const char * _name, const std::function<void(Node<T>&)> render_func)
            {
                return input_pins.insert({ _name, Pin<T>{graph.next_id(), _name, *this, render_func} }).first->second;
            }

            inline Pin<T> add_output_pin(const char * _name, const std::function<void(Node<T>&)> render_func)
            {
                return output_pins.insert({ _name, Pin<T>{graph.next_id(), _name, *this, render_func} }).first->second;
            }

            inline Pin<T>& input_pin(const char * _name) {
                return input_pins.find(_name)->second;
            }

            inline Pin<T>& output_pin(const char * _name) {
                return output_pins.find(_name)->second;
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
                    for (const auto& [_name, pin] : input_pins)
                    {
                        pin.render();
                    }
                }
                {//Output pin
                    for (const auto& [_name, pin] : output_pins)
                    {
                        pin.render();
                    }
                }
                {
                    render_body(data);
                }
                ImNodes::EndNode();
                ImGui::PopID();
                return ImNodes::GetNodeDimensions(id);
            }
        };

        template<typename Tu, typename Tv>
        class ColoredBipartiteGraph : public Widgets::GraphViewer {
        protected:
            std::unordered_map<std::string, Node<Tu>> U {};
            std::unordered_map<std::string, Node<Tv>> V {};
            std::vector<Link<Tu, Tv>> positive_links {};
            std::vector<Link<Tu, Tv>> negative_links {};

            std::unordered_set<std::string> U_active {};
            std::unordered_set<std::string> V_highest {};

            const ImVec2 origin {ImVec2(50.f, 50.f )};
            const ImVec2 offset {ImVec2(200.f, 20.f )};

        public:

            explicit ColoredBipartiteGraph(const char * name) :
                    GraphViewer(name)
            {}

            inline Node<Tu>& add_left_node(const char * name, const Tu* data, std::function<void(const Tu*)> render_body)
            {
                return (U.emplace(name, Node<Tu>{*this, name, data, render_body})).first->second;
            }

            inline Node<Tv>& add_right_node(const char * name, const Tv* data, std::function<void(const Tv*)> render_body)
            {
                return (V.emplace(name, Node<Tv>{*this, name, data, render_body})).first->second;
            }

            inline void link(Pin<Tu>& start_pin, Pin<Tv>& end_pin, bool positive) {
                if (positive) {
                    positive_links.emplace_back(next_id(), start_pin, end_pin);
                }
                else {
                    negative_links.emplace_back(next_id(), start_pin, end_pin);
                }
            }

            inline Node<Tu>& find_left_node_from(const char * name) {
                return U.find(name)->second;
            }

            inline Node<Tv>& find_right_node_from(const char * name) {
                return V.find(name)->second;
            }

            inline std::vector<const Link<Tu, Tv>&> get_in_edges_of(const Node<Tv>& node) {
                std::vector<const Link<Tu, Tv>&> edges;
                for (const Link<Tu, Tv>& link : positive_links) {
                    if (link.end_pin.node.name == node.name)
                        edges.push_back(link);
                }

                for (const Link<Tu, Tv>& link : negative_links) {
                    if (link.end_pin.node.name == node.name)
                        edges.push_back(link);
                }
                return edges;
            }

            inline std::vector<const Link<Tu, Tv>&> get_out_edges_of(const Node<Tu>& node) {
                std::vector<const Link<Tu, Tv>&> edges;
                for (const Link<Tu, Tv>& link : positive_links) {
                    if (link.start_pin.node.name == node.name)
                        edges.push_back(link);
                }

                for (const Link<Tu, Tv>& link : negative_links) {
                    if (link.start_pin.node.name == node.name)
                        edges.push_back(link);
                }
                return edges;
            }

            inline void refresh(const std::unordered_set<std::string>& elements) {
                U_active = std::unordered_set{ elements };
            }

            inline void compute_highest() {
                V_highest.clear();
                int max_value = 0;

                auto highest_elt = std::max_element(V.begin(), V.end(), [](const std::pair<std::string, Node<Tv>>& pair_a, const std::pair<std::string, Node<Tv>>& pair_b) {
                    return pair_a.second.score < pair_b.second.score;
                });
                max_value = highest_elt->second.score;

                for (const auto& [name, node] : V) {
                    if (node.score == max_value)
                        V_highest.insert(name);
                }
            }

            inline void clear_influence() {
                positive_links.clear();
                negative_links.clear();
                V_highest.clear();
                for (auto& [name, node] : V) {
                    node.score = 0;
                }
            }

            inline void clear() {
                U.clear();
                V.clear();
                positive_links.clear();
                negative_links.clear();
                U_active.clear();
                V_highest.clear();
            }

        private:

            void _render() const override {

                ImVec2 cursor(origin);
                ImVec2 dimensions;
                float max_length = 0.f;

                for (const auto& [name, u] : U) {
                    {// SET STYLE
                        if (U_active.contains(name)) {
                            ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::GRAY_n);
                            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::GRAY_h);
                            ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::GRAY_s);
                        }
                        else {
                            ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::Color::BLUE_n);
                            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::Color::BLUE_h);
                            ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImGui::Color::BLUE_s);
                        }
                    }

                    dimensions = u.render();

                    { // SET POSITION
                        ImNodes::SetNodeGridSpacePos(u.id, cursor);
                        ImNodes::SetNodeDraggable(u.id, false);
                        if (max_length < dimensions.x)
                            max_length = dimensions.x;
                        cursor.y += dimensions.y + offset.y;
                    }
                    ImNodes::PopColorStyle();
                    ImNodes::PopColorStyle();
                    ImNodes::PopColorStyle();
                }
                cursor.y = origin.y;
                cursor.x += max_length + offset.x;

                for (const auto& [name, v] : V) {
                    {// SET STYLE
                        if (V_highest.contains(name)) {
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
                    dimensions = v.render();

                    { // SET POSITION
                        ImNodes::SetNodeGridSpacePos(v.id, cursor);
                        ImNodes::SetNodeDraggable(v.id, false);
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
                for (const Link<Tu, Tv>& link : positive_links) {
                    link.render();
                }

                ImNodes::PushColorStyle(ImNodesCol_Link, ImGui::Color::RED_n);
                ImNodes::PushColorStyle(ImNodesCol_LinkHovered, ImGui::Color::RED_h);
                ImNodes::PushColorStyle(ImNodesCol_LinkSelected, ImGui::Color::RED_s);
                for (const Link<Tu, Tv>& link : negative_links) {
                    link.render();
                }
            };
        };
    }

    namespace Graph{
        template<typename T>
        class SimpleGraph : public Widgets::GraphViewer {
        protected:
            ogdf::Graph graph{};
            //std::vector<Flow::Node<T>> nodes {};
            //std::vector<Flow::Link<T, T>> links {};

        public:
            explicit SimpleGraph(const char * name ) : Widgets::GraphViewer(name){}

            Flow::Node<T> add_node(const char * name){
                //nodes.emplace_back(*this, name, nullptr, nullptr);
            }

            Flow::Node<T> link(Flow::Node<T> a, Flow::Node<T> b){
            }

            void clear(){
                //nodes.clear();
                //links.clear();
            }
        };
    }
}
#endif //IMGUI_ADDONS_GRAPH_HPP
