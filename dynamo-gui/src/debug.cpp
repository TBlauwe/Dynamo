#include <dynamo/gui/debug.hpp>
#include <imnodes.h>

void dynamo::gui::show(dynamo::Simulation& sim) {
    ImGui::Begin("Dynamo");
    // 2: Iterate all entities in the world
    for (auto it : sim.world) {
        flecs::type table_type = it.table_type();
        ImGui::Text("Entity : {} , {}", table_type.str().c_str(), it.count());
    }
    ImGui::End();

    ImGui::Begin("simple node editor");
    ImNodes::BeginNodeEditor();
    ImNodes::BeginNode(1);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("simple node :)");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(2);
    ImGui::Text("input");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(3);
    ImGui::Indent(40);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();

    ImNodes::MiniMap(.1f, ImNodesMiniMapLocation_BottomLeft);
    ImNodes::EndNodeEditor();
    ImGui::End();
}
