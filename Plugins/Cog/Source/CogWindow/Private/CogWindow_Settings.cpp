#include "CogWindow_Settings.h"

#include "CogImguiModule.h"
#include "InputCoreTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogWindow_Settings::UCogWindow_Settings()
{
    bHasMenu = false;
    ToggleInputKey.Key = EKeys::Tab;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow_Settings::PostInitProperties() 
{
    Super::PostInitProperties();

    if (ToggleInputKey.Key != EKeys::Invalid)
    {
        FCogImguiModule::Get().SetToggleInputKey(ToggleInputKey);
    }
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

    ImGui::Checkbox("Compact Mode", &GetOwner()->bCompactMode);

    ImGui::Checkbox("Show Windows In Main Menu", &GetOwner()->bShowWindowsInMainMenu);
    
    ImGui::Checkbox("Show Window Help", &GetOwner()->bShowHelp);

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Change DPi Scale [Mouse Wheel]");
        ImGui::TextUnformatted("Reset DPi Scale  [Middle Mouse]");
        ImGui::EndTooltip();
    }

    ImGui::Separator();

    ImGui::Text("Toggle Input Key");

    TArray<FKey> Keys;
    EKeys::GetAllKeys(Keys);

    bool HasKeyChanged = false;
    FCogWindowWidgets::SetNextItemToShortWidth();
    if (ImGui::BeginCombo("Key", TCHAR_TO_ANSI(*ToggleInputKey.Key.ToString())))
    {
        for (int32 i = 0; i < Keys.Num(); ++i)
        {
            const FKey Key = Keys[i];
            if (Key.IsDigital() == false || Key.IsDeprecated())
            {
                continue;
            }

            bool IsSelected = ToggleInputKey.Key == Key;
            if (ImGui::Selectable(TCHAR_TO_ANSI(*Key.ToString()), IsSelected))
            {
                ToggleInputKey.Key = Key;
                HasKeyChanged = true;
            }
        }
        ImGui::EndCombo();
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    HasKeyChanged |= FCogWindowWidgets::ComboboxEnum("Ctrl", ToggleInputKey.Ctrl);

    FCogWindowWidgets::SetNextItemToShortWidth();
    HasKeyChanged |= FCogWindowWidgets::ComboboxEnum("Shift", ToggleInputKey.Shift);
    
    FCogWindowWidgets::SetNextItemToShortWidth();
    HasKeyChanged |= FCogWindowWidgets::ComboboxEnum("Alt", ToggleInputKey.Alt);
    
    FCogWindowWidgets::SetNextItemToShortWidth();
    HasKeyChanged |= FCogWindowWidgets::ComboboxEnum("Cmd", ToggleInputKey.Cmd);

    if (HasKeyChanged)
    {
        FCogImguiModule::Get().SetToggleInputKey(ToggleInputKey);
    }

    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::Button("Reset All Windows Config"))
    {
        GetOwner()->ResetAllWindowsConfig();
    }
}
