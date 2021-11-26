#include <imgui-addons/nodes_editor.hpp>

ImGui::Flow::EditorContext::EditorContext() :
        editor{ImNodes::EditorContextCreate()}
{
    ImNodesStyle& style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_Link]       = ImGui::Color::GRAY_n;
    style.Colors[ImNodesCol_Pin]        = ImGui::Color::GRAY_n;
    style.Colors[ImNodesCol_PinHovered] = ImGui::Color::GRAY_h;
    style.LinkThickness     = 5.f;
    style.PinCircleRadius   = 5.f;
}

ImGui::Flow::EditorContext::EditorContext(ImGui::Flow::EditorContext&& that) : editor{std::exchange(that.editor, nullptr)}
{}

ImGui::Flow::EditorContext& ImGui::Flow::EditorContext::operator=(ImGui::Flow::EditorContext&& that)
{
    std::swap(editor, that.editor);
    return *this;
}

ImGui::Flow::EditorContext::~EditorContext()
{
    if(editor != nullptr)
        ImNodes::EditorContextFree(editor);
}

void ImGui::Flow::EditorContext::begin() const
{
    ImNodes::EditorContextSet(editor);
    ImNodes::BeginNodeEditor();
}

void ImGui::Flow::EditorContext::end() const
{
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);
    ImNodes::EndNodeEditor();
}

ImGui::Flow::Node& ImGui::Flow::Graph::node(const char* name)
{
    return nodes.emplace_back(&id_count, name);
}

void ImGui::Flow::Graph::link(Node* a, const char* output_name, Node* b, const char* input_name)
{
    links.emplace_back(id_count.next_id(), a->output_pin(output_name), b->input_pin(input_name));
}

void ImGui::Flow::Graph::link(const Pin* a, const Pin* b)
{
    links.emplace_back(id_count.next_id(), a->id, b->id);
}

void ImGui::Flow::Graph::render() const
{
    context.begin();
    this->render_graph();
    context.end();
}

inline void ImGui::Flow::Graph::clear()
{
    nodes.clear();
    links.clear();
}