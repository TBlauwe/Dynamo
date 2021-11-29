#include <dynamo/gui/widgets/entity_widgets.hpp>



#include <imgui.h>
#include <IconsFontAwesome5.h>
#include <spdlog/fmt/bundled/format.h>

#include <dynamo/gui/core.hpp>
#include <dynamo/gui/widgets/component_widgets.hpp>

namespace dynamo::widgets {
    void inspect(flecs::entity& entity){
        if(ImGui::CollapsingHeader(fmt::format("Inspector : {}##{}", entity.name(), entity.id()).c_str())){
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.f, 10.f));
            if (ImGui::BeginTable("Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Role", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                entity.each([&entity](flecs::id &id) {
                    ID_TYPE id_type = type_of(id);
                    if(id_type != ID_TYPE::SKIP){
                        ImGui::TableNextRow();
                        switch(id_type){
                            case ID_TYPE::TAG:
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.15f, 0.6f, 0.6f))));
                                ImGui::Text("TAG");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s", id.object().name().c_str());
                                break;
                            case ID_TYPE::COMPONENT:
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.3f, 0.6f, 0.6f))));
                                ImGui::Text("COMPONENT");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s", id.object().name().c_str());
                                break;
                            case ID_TYPE::RELATION:
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.45f, 0.6f, 0.6f))));
                                ImGui::Text("RELATION");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s " ICON_FA_LONG_ARROW_ALT_RIGHT " %s ", id.relation().name().c_str(), id.object().name().c_str());
                                break;
                            case ID_TYPE::IS_A:
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.6f, 0.6f, 0.6f))));
                                ImGui::Text("IS A");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s ", id.object().name().c_str());
                                break;
                            case ID_TYPE::CHILD_OF:
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.75f, 0.6f, 0.6f))));
                                ImGui::Text("CHILD OF");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s ", id.object().name().c_str());
                                break;
                            case ID_TYPE::OWNED:
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.90f, 0.6f, 0.6f))));
                                ImGui::Text("PREFAB");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s ", id.object().name().c_str());
                                break;
                        }
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", id.role_str().c_str());
                        ImGui::TableSetColumnIndex(3);
                        show_component_widget(entity, id_type, id);
                    }
                });
                entity.children([&entity](flecs::entity child) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(static_cast<ImVec4>(ImColor::HSV(0.75f, 0.6f, 0.6f))));
                        ImGui::Text("HAS CHILD");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s ", child.name().c_str());
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("...");
                        ImGui::TableSetColumnIndex(3);
                        inspect(child);
                });
                ImGui::PopStyleVar();
                ImGui::EndTable();
            }
        }
    }

    void show_action_widget(flecs::entity& entity){
        auto gui = entity.get_mut<type::GUI>();
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, &gui->show_widget)){
            ImGui::End();
            return;
        }
        inspect(entity);
        ImGui::End();
    }

    void show_agent_widget(flecs::entity& entity){
        auto gui = entity.get_mut<type::GUI>();
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, &gui->show_widget)){
            ImGui::End();
            return;
        }
        ImGui::BeginGroup();
        ImGui::BeginChild("NodesEditorView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 1));
        if (ImGui::BeginTable("NodesEditor", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Brain viewer", ImGuiTableColumnFlags_WidthStretch, 480);
            ImGui::TableSetupColumn("Process inspector");
            ImGui::TableHeadersRow();
            {
                entity.children([](flecs::entity child)
                    {
                        if (child.has<type::BrainViewer>())
                        {
                            dynamo::widgets::BrainViewer* current_viewer = nullptr;
                            const dynamo::type::ProcessDetails* process_details = nullptr;
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            {
                                if (ImGui::BeginTabBar("ProcessViewer"))
                                {
                                    if (ImGui::BeginTabItem(child.name()))
                                    {
                                        current_viewer = &child.get_mut<type::BrainViewer>()->viewer;
                                        process_details = child.get<type::ProcessDetails>();
                                        current_viewer->render();
                                        ImGui::EndTabItem();
                                    }
                                    ImGui::EndTabBar();
                                }
                            }
                            ImGui::TableSetColumnIndex(1);
                            {
                                const int num_selected_nodes = ImNodes::NumSelectedNodes();
                                if (num_selected_nodes > 0)
                                {
                                    std::vector<int> selected_nodes;
                                    selected_nodes.resize(num_selected_nodes);
                                    ImNodes::GetSelectedNodes(selected_nodes.data());
                                    const ImGui::Flow::Node* node = current_viewer->find_node(selected_nodes[0]);
                                    auto process = process_details->find(current_viewer->find_task(node));
                                    ImGui::Text("Node : %s", node->name);
                                    ImGui::Text("Type : %s", to_string(process.type()));
                                }
                            }
                        }
                    }
                );
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        if (entity.has<type::InfluenceGraphViewer<int>>())
        {
            entity.get_mut<type::InfluenceGraphViewer<int>>()->viewer.render();
        }
        inspect(entity);
        ImGui::EndGroup();
        ImGui::End();
    }

    void show_artefact_widget(flecs::entity& entity){
        auto gui = entity.get_mut<type::GUI>();
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, &gui->show_widget)){
            ImGui::End();
            return;
        }
        inspect(entity);
        ImGui::End();
    }

    void show_percept_widget(flecs::entity& entity){
        auto gui = entity.get_mut<type::GUI>();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin("Percept", &gui->show_widget)){
            ImGui::End();
            return;
        }
        inspect(entity);
        ImGui::End();
    }

    void show_organisation_widget(flecs::entity& entity){
        auto gui = entity.get_mut<type::GUI>();
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, &gui->show_widget)){
            ImGui::End();
            return;
        }
        inspect(entity);
        ImGui::End();
    }
}
