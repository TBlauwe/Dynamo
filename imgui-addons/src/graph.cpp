#include <imgui-addons/graph.hpp>

ImGui::Graph::GraphViewer::GraphViewer() :
        editor{ImNodes::EditorContextCreate()}
{
    ImNodesStyle& style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_Link]       = ImGui::Color::GRAY_n;
    style.Colors[ImNodesCol_Pin]        = ImGui::Color::GRAY_n;
    style.Colors[ImNodesCol_PinHovered] = ImGui::Color::GRAY_h;
    style.LinkThickness     = 5.f;
    style.PinCircleRadius   = 5.f;
}

ImGui::Graph::GraphViewer::GraphViewer(const GraphViewer& that) :
    GraphViewer()
{}

ImGui::Graph::GraphViewer& ImGui::Graph::GraphViewer::operator=(const GraphViewer& that)
{
    return *this;
}

ImGui::Graph::GraphViewer::~GraphViewer() {
    ImNodes::EditorContextFree(editor);
}

void ImGui::Graph::GraphViewer::render() const {
    ImNodes::EditorContextSet(editor);
    ImNodes::BeginNodeEditor();
    _render();
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);
    ImNodes::EndNodeEditor();
}