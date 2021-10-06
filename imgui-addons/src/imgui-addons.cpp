#include <imgui-addons/imgui-addons.hpp>

namespace ImGui::Addons{
    void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled(ICON_FA_QUESTION_CIRCLE);
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void InputFloatColor(float& value){
        float gradient = std::clamp(value, 0.f, 0.3f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(gradient, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(gradient, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(gradient, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(gradient, 0.9f, 0.9f));
        ImGui::InputFloat("", &value, 0.5f, 1.f, "%.3f");
        ImGui::PopStyleColor(4);
    }
}