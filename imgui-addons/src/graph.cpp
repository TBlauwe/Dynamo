#include <imgui-addons/graph.hpp>

ImGui::Widgets::GraphViewer::GraphViewer(const char* name) :
        name{name},
        editor{ImNodes::EditorContextCreate()}
{
    ImNodesStyle& style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_Link] = ImGui::Color::GRAY_n;
    style.Colors[ImNodesCol_Pin] = ImGui::Color::GRAY_n;
    style.Colors[ImNodesCol_PinHovered] = ImGui::Color::GRAY_h;
    style.LinkThickness = 5.f;
    style.PinCircleRadius = 5.f;
}

ImGui::Widgets::GraphViewer::~GraphViewer() {
    ImNodes::EditorContextFree(editor);
}

void ImGui::Widgets::GraphViewer::render() const {
    ImNodes::EditorContextSet(editor);
    ImNodes::BeginNodeEditor();
    _render();
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);
    ImNodes::EndNodeEditor();
}