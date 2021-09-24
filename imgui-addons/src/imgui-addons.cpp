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
}