#include "CogWindow_Spacing.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Spacing::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Spacing::PostBegin()
{
    ImGui::PopStyleColor(1);
}
