#include <dynamo/gui/dynamo_inspector.hpp>
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

        world.system<const dynamo::type::Agent>("AddAgentToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::type::Agent& agent){
                    e.set<component::GUI>({});
                });

        world.system<const dynamo::type::Artefact>("AddArtefactToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::type::Artefact& artefact){
                    e.set<component::GUI>({});
                });

        world.system<const dynamo::type::Percept>("AddPerceptToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::type::Percept& percept){
                    e.set<component::GUI>({});
                });

        world.system<const dynamo::type::Organisation>("AddOrganisationToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::type::Organisation& organisation){
                    e.set<component::GUI>({});
                });

        world.system<const dynamo::type::Action>("AddActionToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::type::Action& action){
                    e.set<component::GUI>({});
                });

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
                            dynamo_gui::widget::show_action_widget(&gui.show_widget, e);
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
                            dynamo_gui::widget::show_agent_widget(&gui.show_widget, e);
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
                            dynamo_gui::widget::show_artefact_widget(&gui.show_widget, e);
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
                                    dynamo_gui::widget::show_organisation_widget(&gui.show_widget, e);
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
                ImGui::TableSetupColumn("TTL", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                percepts_query.each([this](flecs::entity e, const dynamo::type::Percept &agent, component::GUI& gui) {
                    if (agents_list_filter.PassFilter(e.name())) {
                        ImGui::PushID(static_cast<int>(e.id()));
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        e.each<dynamo::relation::source>([](flecs::entity obj) {
                            ImGui::Text("%s", obj.name().c_str());
                        });

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", e.name().c_str());

                        ImGui::TableSetColumnIndex(2);
                        if(e.has<::dynamo::component::Decay>()){
                            auto* decay = e.get_mut<::dynamo::component::Decay>();
                            ImGui::SetNextItemWidth(100);
                            float min = 0;
                            float max = 2;
                            float gradient = decay->ttl / (max * 3);
                            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(gradient, 0.5f, 0.5f));
                            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(gradient, 0.6f, 0.5f));
                            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(gradient, 0.7f, 0.5f));
                            ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(gradient, 0.9f, 0.9f));
                            ImGui::SliderFloat("", &(*decay).ttl, min, max, "%.3f", ImGuiSliderFlags_ReadOnly);
                            ImGui::PopStyleColor(4);
                        }

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                        if (gui.show_widget) {
                            dynamo_gui::widget::show_percept_widget(&gui.show_widget, e);
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
