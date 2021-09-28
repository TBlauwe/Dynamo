#include <dynamo/gui/debug.hpp>
#include <dynamo/gui/widgets/entity_widgets.hpp>
#include <imnodes.h>
#include <misc/cpp/imgui_stdlib.h>
#include <IconsFontAwesome5.h>

namespace dynamo_gui{

    Inspector::Inspector(dynamo::Simulation &sim):sim{sim} {
        agents_query = sim.world.query<const dynamo::tag::Agent, component::GUI>();
        artefacts_query = sim.world.query<const dynamo::tag::Artefact, component::GUI>();
        percepts_query = sim.world.query<const dynamo::tag::Percept, component::GUI>();
        organisations_query = sim.world.query<const dynamo::tag::Organisation, component::GUI>();
        actions_query = sim.world.query<const dynamo::tag::Action, component::GUI>();

        sim.world.system<const dynamo::tag::Agent>("AddAgentToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::tag::Agent& agent){
                    e.set<component::GUI>({});
                });

        sim.world.system<const dynamo::tag::Artefact>("AddArtefactToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::tag::Artefact& artefact){
                    e.set<component::GUI>({});
                });

        sim.world.system<const dynamo::tag::Percept>("AddPerceptToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::tag::Percept& percept){
                    e.set<component::GUI>({});
                });

        sim.world.system<const dynamo::tag::Organisation>("AddOrganisationToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::tag::Organisation& organisation){
                    e.set<component::GUI>({});
                });

        sim.world.system<const dynamo::tag::Action>("AddActionToGui")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const dynamo::tag::Action& action){
                    e.set<component::GUI>({});
                });

        timescale = sim.world.get_time_scale();
    }

    void Inspector::show() {
        ImGui::Begin("Dynamo-Bar");
        if(ImGui::Button(!is_enabled ? ICON_FA_PLAY " Play" : ICON_FA_PAUSE " Pause")){
            is_enabled = !is_enabled;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        if(ImGui::SliderFloat("TimeScale", &timescale, 0.1f, 5.f)){
            sim.world.set_time_scale(timescale);
        }
        ImGui::SameLine();
        ImGui::Addons::HelpMarker("Ctrl + Click to input a specific value");
        ImGui::End();

        static std::string event = "Some event";
        ImGui::Begin("Dynamo");
        ImGui::InputText("Event", &event);
        if(ImGui::Button("Add Event")){
            sim.add_event<dynamo::event::MAJOR>(event.c_str());
        }
        ImGui::End();

        // ===== Update stats =====
        if(is_enabled){
            scrolling_plot_percepts.add(sim.world.count<const dynamo::tag::Percept>());
            scrolling_plot_delta_time.add(sim.world.delta_time());
        }

        // ===== Display stats =====
        ImGui::Begin("Dynamo");
        if(ImGui::BeginTabBar("DynamoTabBar")) {
            if (ImGui::BeginTabItem("Overall")) {
                ImPlot::SetNextPlotLimits(0, scrolling_plot_delta_time.capacity(), 0, 0.5, ImGuiCond_Once);
                if (ImPlot::BeginPlot("Delta-time", nullptr, nullptr, {-1, 0}, ImPlotFlags_NoTitle,
                                      ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_LockMin)) {
                    scrolling_plot_delta_time.plot();
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Perceptions")) {
                ImPlot::SetNextPlotLimits(0, scrolling_plot_delta_time.capacity(), 0, 100, ImGuiCond_Once);
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
                    percepts_query.each([this](flecs::entity e, const dynamo::tag::Percept &agent, component::GUI& gui) {
                        if (agents_list_filter.PassFilter(e.name())) {
                            ImGui::PushID(static_cast<int>(e.id()));
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            e.each<dynamo::relation::from>([](flecs::entity obj) {
                                ImGui::Text("%s", obj.name().c_str());
                            });

                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", e.name().c_str());

                            ImGui::TableSetColumnIndex(2);
                            if(e.has<::dynamo::component::DecayingPercept>()){
                                auto* decaying_percept = e.get_mut<::dynamo::component::DecayingPercept>();
                                ImGui::SetNextItemWidth(100);
                                float min = 0;
                                float max = 2;
                                float gradient = decaying_percept->ttl / (max * 3);
                                ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(gradient, 0.5f, 0.5f));
                                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(gradient, 0.6f, 0.5f));
                                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(gradient, 0.7f, 0.5f));
                                ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(gradient, 0.9f, 0.9f));
                                ImGui::SliderFloat("", &(*decaying_percept).ttl, min, max, "%.3f", ImGuiSliderFlags_ReadOnly);
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
            if (ImGui::BeginTabItem("Agents")) {
                agents_list_filter.Draw();
                ImGui::Spacing();
                if (ImGui::BeginTable("Agents##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    agents_query.each([this](flecs::entity e, const dynamo::tag::Agent &agent, component::GUI& gui) {
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
            if (ImGui::BeginTabItem("Artefacts")) {
                artefacts_list_filter.Draw();
                ImGui::Spacing();
                if (ImGui::BeginTable("Artefacts##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    artefacts_query.each([this](flecs::entity e, const dynamo::tag::Artefact &artefact, component::GUI& gui) {
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

            if (ImGui::BeginTabItem("Organisations")) {
                organisations_list_filter.Draw();
                ImGui::Spacing();
                if (ImGui::BeginTable("Organisations##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    organisations_query.each([this](flecs::entity e, const dynamo::tag::Organisation &organisation, component::GUI& gui) {
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
            if (ImGui::BeginTabItem("Actions")) {
                actions_list_filter.Draw();
                ImGui::Spacing();
                if (ImGui::BeginTable("Organisations##table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    actions_query.each([this](flecs::entity e, const dynamo::tag::Action &action, component::GUI& gui){
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
            ImGui::EndTabBar();
        }
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
        ImGui::End();
    }
}