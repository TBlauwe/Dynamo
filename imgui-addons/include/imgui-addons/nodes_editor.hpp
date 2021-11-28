#ifndef IMGUI_ADDONS_NODES_EDITOR_HPP
#define IMGUI_ADDONS_NODES_EDITOR_HPP


#include <string>
#include <vector>
#include <unordered_set>

#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <imnodes.h>

#include <imgui-addons/imgui-addons.hpp>

namespace ImGui{
    namespace Flow {

        struct Counter
        {
            size_t count{ 0 };

            inline size_t next_id() { return count++; }
        };

        class EditorContext {
            friend class Graph;
            friend class Node;

        public:
            explicit EditorContext(); 
            EditorContext(const EditorContext& that) = delete;
            EditorContext(EditorContext&& that);
            EditorContext& operator=(const EditorContext& that) = delete;
            EditorContext& operator=(EditorContext&& that);
            ~EditorContext();                        

        public:
            void begin() const;
            void end() const;

        public:
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

            Link(const size_t id, const size_t start, const size_t end) :
                id{ id },
                start{ start },
                end{ end }
            {}

            inline void render() const { 
                ImNodes::Link(id, start, end); 
            }
        };

        struct Node {

            Counter*        counter;
            size_t          id;
            const char*     name;
            std::list<Pin>	input_pins{};
            std::list<Pin>	output_pins{};

            Node(Counter* counter, const char * name) :
                counter { counter },
                id      { counter->next_id()},
                name    { name }
            {}

            inline Pin& input_pin(const char * _name)
            {
                return input_pins.emplace_back(counter->next_id(), _name);
            }

            inline Pin& output_pin(const char * _name)
            {
                return output_pins.emplace_back(counter->next_id(), _name);
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
    //        };
    //    };
    //}

        class Graph
        {
        public:
            Graph() = default;

            Node& node(const char* name);
            void link(Node* a, const char* output_name, Node* b, const char* input_name);
            void link(const Pin* a, const Pin* b);
            void clear();
            void render() const;

        private:
            virtual void render_graph() const = 0;

        protected:
            EditorContext       context {};
            Counter             id_count{};
            std::list<Node>     nodes {};
            std::vector<Link>   links {};
        };

        class BipartiteGraph
        {
        public:
            BipartiteGraph() = default;

            Node& left_node(const char* name);
            Node& right_node(const char* name);
            void positive_link(const Node* a, const Node* b);
            void negative_link(const Node* a, const Node* b);
            void render() const;

        private:
            virtual void render_graph() const = 0;

        protected:
            EditorContext       context {};
            Counter             id_count{};
            std::list<Node>     left_nodes {};
            std::list<Node>     right_nodes {};
            std::vector<Link>   positive_links {};
            std::vector<Link>   negative_links {};
        };
    }
}
#endif //IMGUI_ADDONS_GRAPH_HPP
