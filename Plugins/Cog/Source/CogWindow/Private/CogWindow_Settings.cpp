#include "CogWindow_Settings.h"

#include "CogImguiModule.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "InputCoreTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogWindow_Settings::UCogWindow_Settings()
{
    bHasMenu = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow_Settings::PostInitProperties() 
{
    Super::PostInitProperties();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow_Settings::RenderContent()
{
    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat("DPI Scale", &GetOwner()->DPIScale, 0.5f, 2.0f, "%.1f");
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        GetOwner()->bRefreshDPIScale = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Change DPi Scale [Mouse Wheel]");
        ImGui::TextUnformatted("Reset DPi Scale  [Middle Mouse]");
        ImGui::EndTooltip();
    }

    ImGui::Checkbox("Compact Mode", &GetOwner()->bCompactMode);

    ImGui::Checkbox("Show Windows In Main Menu", &GetOwner()->bShowWindowsInMainMenu);
    
    ImGui::Checkbox("Show Window Help", &GetOwner()->bShowHelp);

    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::Button("Reset All Windows Config"))
    {
        GetOwner()->ResetAllWindowsConfig();
    }
}
