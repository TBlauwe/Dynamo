#include <dynamo/gui/dynamo_inspector.hpp>
#include <dynamo/gui/widgets/component_widgets.hpp>
#include <dynamo/gui/widgets/entity_widgets.hpp>
#include <imnodes.h>
#include <IconsFontAwesome5.h>

struct tag{};

namespace dynamo{

    DynamoInspector::DynamoInspector(Simulation& sim):
        sim{sim},
        world{sim.world()}
    {
        world.set<type::ActiveTasks>({ sim.executor.make_observer<TasksObs>() });
        agents_query        = world.query<const type::Agent, type::GUI>();
        artefacts_query     = world.query<const type::Artefact, type::GUI>();
        percepts_query      = world.query<const type::Percept, type::GUI>();
        organisations_query = world.query<const type::Organisation, type::GUI>();
        actions_query       = world.query<const type::Action, type::GUI>();

        add_tag_to<type::GUI, type::Action>(world, "GUI", "Action");
        add_tag_to<type::GUI, type::Agent>(world, "GUI", "Agent");
        add_tag_to<type::GUI, type::Artefact>(world, "GUI", "Artefact");
        add_tag_to<type::GUI, type::Organisation>(world, "GUI", "Organisation");
        add_tag_to<type::GUI, type::Percept>(world, "GUI", "Percept");


        world.observer<type::ProcessHandle>("OnSet_Process_AddNodeToBrainViewer")
            .event(flecs::OnSet)
            .each([](flecs::entity e, type::ProcessHandle& handle) {
                auto parent = e.get_object(flecs::ChildOf);
                if (parent.has(flecs::Prefab))
                    return;
               parent.set<type::BrainViewer>({parent, handle.taskflow});
            });

        world.system<const type::Percept>("UpdatePlot_PerceptsCount")
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

        actions_query.each([this](flecs::entity e, const type::Action& action, type::GUI& gui) {
                if (gui.show_widget) {
                    ImGui::PushID(static_cast<int>(e.id()));
                    widgets::show_action_widget(e);
                    ImGui::PopID();
                }
           });

        agents_query.each([this](flecs::entity e, const type::Agent& agent, type::GUI& gui) {
                if (gui.show_widget) {
                    ImGui::PushID(static_cast<int>(e.id()));
                    widgets::show_agent_widget(e);
                    ImGui::PopID();
                }
            });

        artefacts_query.each([this](flecs::entity e, const type::Artefact& artefact, type::GUI& gui) {
                if (gui.show_widget) {
                    ImGui::PushID(static_cast<int>(e.id()));
                    widgets::show_artefact_widget(e);
                    ImGui::PopID();
                }
            });

        organisations_query.each([this](flecs::entity e, const type::Organisation& organisation, type::GUI& gui) {
                    if (gui.show_widget) {
                        ImGui::PushID(static_cast<int>(e.id()));
                        widgets::show_organisation_widget(e);
                        ImGui::PopID();
                    }
            });

        percepts_query.each([this](flecs::entity e, const type::Percept& agent, type::GUI& gui) {
                if (gui.show_widget) {
                    ImGui::PushID(static_cast<int>(e.id()));
                    widgets::show_percept_widget(e);
                    ImGui::PopID();
                }
            });

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
                actions_query.each([this](flecs::entity e, const type::Action &action, type::GUI& gui){
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
                agents_query.each([this](flecs::entity e, const type::Agent &agent, type::GUI& gui) {
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
                artefacts_query.each([this](flecs::entity e, const type::Artefact &artefact, type::GUI& gui) {
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
                        [this](flecs::entity e, const type::Organisation &organisation, type::GUI &gui) {
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
                percepts_query.each([this](flecs::entity e, const type::Percept &agent, type::GUI& gui) {
                    if (percepts_list_filter.PassFilter(e.name())) {
                        ImGui::PushID(static_cast<int>(e.id()));
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        e.each<type::source>([](flecs::entity obj) {
                            ImGui::Text("%s", obj.name().c_str());
                        });

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", e.name().c_str());

                        ImGui::TableSetColumnIndex(2);
                        if(e.has<type::Decay>()){
                            widgets::show<type::Decay>(e);
                        }

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Checkbox(ICON_FA_EXTERNAL_LINK_ALT, &gui.show_widget);
                        ImGui::PopID();
                    }
                });
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }
}
