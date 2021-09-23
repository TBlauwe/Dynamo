#include <dynamo/gui/debug.hpp>
#include <imnodes.h>
#include <misc/cpp/imgui_stdlib.h>
#include <IconsFontAwesome5.h>

namespace dynamo::gui{

    MainWindow::MainWindow(Simulation &sim):sim{sim} {}

    void MainWindow::show() {
        ImGui::Begin("Dynamo-Bar");
            ImGui::Text("Ticks : [%d]", sim.world.get_tick());
            ImGui::SameLine();
            if(ImGui::Button(!is_enabled ? ICON_FA_PLAY " Play" : ICON_FA_PAUSE " Pause")){
                is_enabled = !is_enabled;
            }
        ImGui::End();

        static std::string event = "Some event";
        ImGui::Begin("Dynamo");
            ImGui::InputText("Event", &event);
            if(ImGui::Button("Add Event")){
                sim.add_event(event.c_str());
            }
        ImGui::End();

        // ===== Update stats =====
        if(is_enabled){
            bool add_event = sim.world.has<component::Event>();
            std::string event_name = add_event ? sim.world.get<component::Event>()->name : "";
            if(add_event){
                sim.world.remove<component::Event>();
            }
            scrolling_plot_percepts.push({add_event, event_name, sim.world.count<const dynamo::tag::Percept>()});
            scrolling_plot_delta_time.push({add_event, event_name, sim.world.delta_time()});
        }

        // ===== Display stats =====
        ImGui::Begin("Dynamo");
            if(ImGui::BeginTabBar("DynamoTabBar")){
                if(ImGui::BeginTabItem("Overall")){
                    ImPlot::SetNextPlotLimits(0, scrolling_plot_delta_time.capacity(), 0, 0.5, ImGuiCond_Once);
                    if (ImPlot::BeginPlot("Delta-time", "Tick")) {
                        scrolling_plot_delta_time.plot();
                        ImPlot::EndPlot();
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Perceptions")){
                    ImPlot::SetNextPlotLimits(0, scrolling_plot_delta_time.capacity(), 0, 100, ImGuiCond_Once);
                    if (ImPlot::BeginPlot("Perception", "Tick")) {
                        ImPlot::NextColormapColor();
                        scrolling_plot_percepts.plot();
                        ImPlot::EndPlot();
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Agents")){
                    if(ImGui::BeginTabBar("AgentsTabBar")) {
                        sim.agents_query.each([](flecs::entity entity_agent, const tag::Agent& agent) {
                            ImGui::PushID(entity_agent);
                            if (ImGui::BeginTabItem(entity_agent.name())) {
                                entity_agent.each<relation::sees>([](flecs::entity obj){
                                    ImGui::BulletText("I'm seeing %d", obj.get<dynamo::component::Integer>()->value);
                                });
                                ImGui::EndTabItem();
                            }
                            ImGui::PopID();
                        });
                        ImGui::EndTabBar();
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
