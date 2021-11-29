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

ImGui::Flow::Node& ImGui::Flow::Graph::node(std::string name)
{
    return nodes.emplace_back(&id_count, name);
}

void ImGui::Flow::Graph::link(Node* a, std::string output_name, Node* b, std::string input_name)
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

void ImGui::Flow::Graph::clear()
{
    id_count.reset();
    nodes.clear();
    links.clear();
}

ImGui::Flow::Node& ImGui::Flow::BipartiteGraph::left_node(std::string name)
{
    return left_nodes.emplace_back(&id_count, name);
}

ImGui::Flow::Node& ImGui::Flow::BipartiteGraph::right_node(std::string name)
{
    return right_nodes.emplace_back(&id_count, name);
}

void ImGui::Flow::BipartiteGraph::positive_link(const ImGui::Flow::Node* a, const ImGui::Flow::Node* b)
{
    positive_links.emplace_back(id_count.next_id(), a->output_pins.front().id, b->input_pins.front().id);
}

void ImGui::Flow::BipartiteGraph::negative_link(const ImGui::Flow::Node* a, const ImGui::Flow::Node* b)
{
    negative_links.emplace_back(id_count.next_id(), a->output_pins.front().id, b->input_pins.front().id);
}

void ImGui::Flow::BipartiteGraph::render() const
{
    context.begin();
    this->render_graph();
    context.end();
}

void ImGui::Flow::BipartiteGraph::clear()
{
    id_count.reset();
    right_nodes.clear();
    left_nodes.clear();
    positive_links.clear();
    negative_links.clear();
}
