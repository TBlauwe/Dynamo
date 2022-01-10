#ifndef DYNAMO_GUI_BRAIN_VIEWER_HPP
#define DYNAMO_GUI_BRAIN_VIEWER_HPP

#include <taskflow/taskflow.hpp>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

#include <imgui-addons/nodes_editor.hpp>
#include <dynamo/internal/process.hpp>

namespace dynamo::widgets {
    class BrainViewer : public ImGui::Flow::Graph
    {
    public:
        BrainViewer() = default;
        BrainViewer(flecs::entity, const tf::Taskflow*, const type::ProcessDetails*);

        const ImGui::Flow::Node* find_node(int i) const;
        size_t find_task(const ImGui::Flow::Node* const) const;

    private:
        void render_graph() const override;
        void build();

    private:
        flecs::entity entity{};
        const tf::Taskflow* taskflow{ nullptr };
        const type::ProcessDetails* _details{ nullptr };

        std::unordered_map<ImGui::Flow::Node const*, size_t> imnodes_hash{};
        std::unordered_map<size_t, ImGui::Flow::Node*> hash_imnodes{};
        std::unordered_map<ImGui::Flow::Node const*, ImVec2> imnodes_pos{};
    };
}

#endif //DYNAMO_GUI_BRAIN_VIEWER_HPP
