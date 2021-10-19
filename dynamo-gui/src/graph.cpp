#include <dynamo/gui/widgets/graph.hpp>

dynamo::gui::widgets::GraphViewer::GraphViewer(const char* name) :
    name{name},
    editor{ImNodes::EditorContextCreate()}
    {}

dynamo::gui::widgets::GraphViewer::~GraphViewer() {
    ImNodes::EditorContextFree(editor);
}

void dynamo::gui::widgets::GraphViewer::render() const {
    ImNodes::EditorContextSet(editor);
    if (ImGui::Begin(name))
    {
        ImNodes::BeginNodeEditor();
        {
            _render();
            ImNodes::EndNodeEditor();
        }
    }
    ImGui::End();
}
