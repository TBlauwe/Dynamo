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
