#include <dynamo/gui/dynamo_inspector.hpp>
#include <dynamo/gui/widgets/component_widgets.hpp>
#include <dynamo/gui/widgets/entity_widgets.hpp>
#include <imnodes.h>
#include <IconsFontAwesome5.h>

namespace dynamo_gui{

    DynamoInspector::DynamoInspector(flecs::world& world):
            world{world}
    {
        agents_query = world.query<const dynamo::type::Agent, component::GUI>();
        artefacts_query = world.query<const dynamo::type::Artefact, component::GUI>();
        percepts_query = world.query<const dynamo::type::Percept, component::GUI>();
        organisations_query = world.query<const dynamo::type::Organisation, component::GUI>();
        actions_query = world.query<const dynamo::type::Action, component::GUI>();

        dynamo::add_tag_to<component::GUI, dynamo::type::Action>(world, "GUI", "Action");
        dynamo::add_tag_to<component::GUI, dynamo::type::Agent>(world, "GUI", "Agent");
        dynamo::add_tag_to<component::GUI, dynamo::type::Artefact>(world, "GUI", "Artefact");
        dynamo::add_tag_to<component::GUI, dynamo::type::Organisation>(world, "GUI", "Organisation");
        dynamo::add_tag_to<component::GUI, dynamo::type::Percept>(world, "GUI", "Percept");

        //world.observer<dynamo::type::Agent>("OnAdd_Agent_AddBrainViewer")
        //        .event(flecs::OnAdd)
        //        .each([](flecs::entity e, const dynamo::type::Agent& _){
        //            e.set<widget::BrainViewer>(widget::BrainViewer{e.name().c_str()});
        //        });

        world.system<const dynamo::type::Percept>("UpdatePlot_PerceptsCount")
                .kind(flecs::PreStore)
                .iter([this](flecs::iter& iter){
                    scrolling_plot_percepts.add(iter.count());
                });
    }

    void DynamoInspector::show() {
        if(!ImGui::Begin("Dynamo")){
            ImGui::End();
            return;
        }

        if(ImGui::BeginTabBar("DynamoTabBar")) {
            show_actions_panel();
            show_agents_panel();
            show_artefacts_panel();
            show_organisations_panel();
            show_percepts_panel();
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    void DynamoInspector::show_actions_panel() {
        if (ImGui::BeginTabItem("Actions")) {
            actions_list_filter.Draw();
            ImGui::Spacing();
            if (ImGui::BeginTable("Organisations##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                actions_query.each([this](flecs::entity e, const dynamo::type::Action &action, component::GUI& gui){
                    ImGui::PushID(static_cast<int>(e.id()));
                    if (actions_list_filter.PassFilter(e.name())) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%llu", e.id());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", e.name().c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", "...");

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                        if (gui.show_widget) {
                            dynamo_gui::widget::show_action_widget(e);
                        }
                    }
                    ImGui::PopID();
                });
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }

    void DynamoInspector::show_agents_panel() {
        if (ImGui::BeginTabItem("Agents")) {
            agents_list_filter.Draw();
            ImGui::Spacing();
            if (ImGui::BeginTable("Agents##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                agents_query.each([this](flecs::entity e, const dynamo::type::Agent &agent, component::GUI& gui) {
                    ImGui::PushID(static_cast<int>(e.id()));
                    if (agents_list_filter.PassFilter(e.name())) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%llu", e.id());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", e.name().c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", "...");

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                        if (gui.show_widget) {
                            dynamo_gui::widget::show_agent_widget(e);
                        }
                    }
                    ImGui::PopID();
                });
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }

    void DynamoInspector::show_artefacts_panel() {
        if (ImGui::BeginTabItem("Artefacts")) {
            artefacts_list_filter.Draw();
            ImGui::Spacing();
            if (ImGui::BeginTable("Artefacts##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                artefacts_query.each([this](flecs::entity e, const dynamo::type::Artefact &artefact, component::GUI& gui) {
                    ImGui::PushID(static_cast<int>(e.id()));
                    if (artefacts_list_filter.PassFilter(e.name())) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%llu", e.id());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", e.name().c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", "...");

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                        if (gui.show_widget) {
                            dynamo_gui::widget::show_artefact_widget(e);
                        }
                    }
                    ImGui::PopID();
                });
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }

    void DynamoInspector::show_organisations_panel() {
        if (ImGui::BeginTabItem("Organisations")) {
            organisations_list_filter.Draw();
            ImGui::Spacing();
            if (ImGui::BeginTable("Organisations##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                organisations_query.each(
                        [this](flecs::entity e, const dynamo::type::Organisation &organisation, component::GUI &gui) {
                            ImGui::PushID(static_cast<int>(e.id()));
                            if (organisations_list_filter.PassFilter(e.name())) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("%llu", e.id());

                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s", e.name().c_str());

                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("%s", "...");

                                ImGui::TableSetColumnIndex(3);
                                ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                                if (gui.show_widget) {
                                    dynamo_gui::widget::show_organisation_widget(e);
                                }
                            }
                            ImGui::PopID();
                        });
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }

    void DynamoInspector::show_percepts_panel() {
        if (ImGui::BeginTabItem("Perceptions")) {
            ImPlot::SetNextPlotLimits(0, scrolling_plot_percepts.capacity(), 0, 100, ImGuiCond_Once);
            if (ImPlot::BeginPlot("Perceptions", nullptr, nullptr, {-1, 0}, ImPlotFlags_NoTitle,
                                  ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_LockMin)) {
                ImPlot::NextColormapColor();
                scrolling_plot_percepts.plot();
                ImPlot::EndPlot();
            }
            percepts_list_filter.Draw();
            ImGui::Spacing();
            if (ImGui::BeginTable("Percepts##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("From", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("TTL", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                percepts_query.each([this](flecs::entity e, const dynamo::type::Percept &agent, component::GUI& gui) {
                    if (agents_list_filter.PassFilter(e.name())) {
                        ImGui::PushID(static_cast<int>(e.id()));
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        e.each<dynamo::type::source>([](flecs::entity obj) {
                            ImGui::Text("%s", obj.name().c_str());
                        });

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", e.name().c_str());

                        ImGui::TableSetColumnIndex(2);
                        if(e.has<dynamo::type::Decay>()){
                            widget::show<dynamo::type::Decay>(e);
                        }

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                        if (gui.show_widget) {
                            dynamo_gui::widget::show_percept_widget(e);
                        }
                        ImGui::PopID();
                    }
                });
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }

    void DynamoInspector::show_node_editor() {
        if(!ImGui::Begin("simple node editor")) {
            ImGui::End();
            return;
        }

        ImNodes::BeginNodeEditor();
        {
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
        }
        ImNodes::MiniMap(.1f, ImNodesMiniMapLocation_BottomLeft);
        ImNodes::EndNodeEditor();
        ImGui::End();
    }
}
