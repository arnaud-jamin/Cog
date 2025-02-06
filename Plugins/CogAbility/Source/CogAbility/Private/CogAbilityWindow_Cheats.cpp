#include "CogAbilityWindow_Cheats.h"

#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::RenderHelp()
{
    ImGui::TextDisabled("This window is deprecated. Please use the CogEngineWindow_Cheat instead as it provide more functionalities.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::RenderContent()
{
    Super::RenderContent();

    RenderHelp();
}