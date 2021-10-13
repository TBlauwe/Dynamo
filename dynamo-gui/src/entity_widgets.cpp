#include <dynamo/gui/core.hpp>
#include <dynamo/gui/widgets/entity_widgets.hpp>
#include <dynamo/gui/widgets/component_widgets.hpp>
#include <imgui.h>
#include <IconsFontAwesome5.h>
#include <spdlog/fmt/bundled/format.h>


namespace dynamo_gui::widget {
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
                ImGui::PopStyleVar();
                ImGui::EndTable();
            }
        }
    }

    void show_action_widget(flecs::entity& entity){
        auto gui = entity.get_mut<dynamo_gui::component::GUI>();
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
        auto gui = entity.get_mut<dynamo_gui::component::GUI>();
        const char * name = entity.name();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(name, &gui->show_widget)){
            ImGui::End();
            return;
        }
        inspect(entity);
        ImGui::End();
    }

    void show_artefact_widget(flecs::entity& entity){
        auto gui = entity.get_mut<dynamo_gui::component::GUI>();
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
        auto gui = entity.get_mut<dynamo_gui::component::GUI>();
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin("Percept", &gui->show_widget)){
            ImGui::End();
            return;
        }
        inspect(entity);
        ImGui::End();
    }

    void show_organisation_widget(flecs::entity& entity){
        auto gui = entity.get_mut<dynamo_gui::component::GUI>();
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
