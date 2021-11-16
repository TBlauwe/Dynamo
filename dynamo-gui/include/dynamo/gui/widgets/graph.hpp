#ifndef DYNAMO_GRAPH_HPP
#define DYNAMO_GRAPH_HPP

#include <imgui-addons/graph.hpp>
#include <dynamo/strategies/influence_graph.hpp>
#include <taskflow/taskflow.hpp>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

namespace dynamo::widgets {
    class BrainViewer : public ImGui::Graph::SimpleGraph<int>{
        tf::Taskflow* last_taskflow {nullptr};

    public:
        explicit BrainViewer(const char * name) : ImGui::Graph::SimpleGraph<int>(name){}

        void compute(tf::Taskflow* taskflow);

    private:
        void compute_graph(tf::Taskflow* taskflow);
        void _render() const override ;
    };

    template<typename Tu, typename Tv>
    class InfluenceGraphViewer : public ImGui::Flow::ColoredBipartiteGraph<Tu, Tv> {
    private:
        const InfluenceGraph<Tu, Tv> *influence_graph;
        std::function<const char *(const Tu &)> get_Tu_name;
        std::function<const char *(const Tv &)> get_Tv_name;
        std::function<void(ImGui::Flow::Node<Tu> &)> render_Tu_node;
        std::function<void(ImGui::Flow::Node<Tv> &)> render_Tv_node;

    public:
        InfluenceGraphViewer(const char *name,
                             std::function<const char *(const Tu &)> get_Tu_name,
                             std::function<const char *(const Tv &)> get_Tv_name,
                             std::function<void(ImGui::Flow::Node<Tu> &)> render_Tu_node,
                             std::function<void(ImGui::Flow::Node<Tv> &)> render_Tv_node
        ) :
                ImGui::Flow::ColoredBipartiteGraph<Tu, Tv>{name},
                influence_graph{nullptr},
                get_Tu_name{get_Tu_name},
                get_Tv_name{get_Tv_name},
                render_Tu_node{render_Tu_node},
                render_Tv_node{render_Tv_node} {
        }

        void show(const InfluenceGraph<Tu, Tv> *graph) {
            influence_graph = graph;
            this->clear();

            for (const auto &u: influence_graph->U) {
                ImGui::Flow::Node<Tu> &node = this->add_left_node(get_Tu_name(u), &u, [](const Tu *u) {});
                node.add_output_pin("output", render_Tu_node);
            }

            for (const auto &v: influence_graph->V) {
                ImGui::Flow::Node<Tv> &node = this->add_right_node(get_Tv_name(v), &v, [](const Tv *v) {});
                node.add_input_pin("input", render_Tv_node);
            }

            for (const auto &influence: influence_graph->positive_influences) {
                auto &u = this->find_left_node_from(get_Tu_name(*influence.start));
                auto &v = this->find_right_node_from(get_Tv_name(*influence.end));
                this->link(u.output_pin("output"), v.input_pin("input"), true);
            }

            for (const auto &influence: influence_graph->negative_influences) {
                auto &u = this->find_left_node_from(get_Tu_name(*influence.start));
                auto &v = this->find_right_node_from(get_Tv_name(*influence.end));
                this->link(u.output_pin("output"), v.input_pin("input"), false);
            }

            this->compute_highest();
        }
    };
}

#endif //DYNAMO_GRAPH_HPP
