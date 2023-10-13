#include "CogWindow_Spacing.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow_Spacing::PreRender(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow_Spacing::PostRender()
{
    ImGui::PopStyleColor(1);
}
