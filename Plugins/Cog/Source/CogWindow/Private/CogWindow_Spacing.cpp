#include "CogWindow_Spacing.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Spacing::PreRender(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Spacing::PostRender()
{
    ImGui::PopStyleColor(1);
}
