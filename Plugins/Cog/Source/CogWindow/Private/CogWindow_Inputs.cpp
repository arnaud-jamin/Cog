#include "CogWindow_Inputs.h"

#include "CogImguiInputHelper.h"
#include "CogImguiWidget.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "InputCoreTypes.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Inputs::Initialize()
{
    Super::Initialize();

    bHasMenu = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Inputs::RenderContent()
{
    const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*GetWorld());
    if (PlayerInput == nullptr)
    {
        return;
    }

    SCogImguiWidget* ImGuiWidget = GetOwner()->GetImGuiWidget().Get();
    if (ImGuiWidget == nullptr)
    {
        return;
    }

    bool bEnableInput = ImGuiWidget->GetEnableInput();
    if (ImGui::Checkbox("Enable Input", &bEnableInput))
    {
        ImGuiWidget->SetEnableInput(bEnableInput);
    }

    const char* ShortcutText = TCHAR_TO_ANSI(*FCogImguiInputHelper::CommandToString(PlayerInput, UCogWindowManager::ToggleInputCommand));
    const float ShortcutWidth = (ShortcutText && ShortcutText[0]) ? ImGui::CalcTextSize(ShortcutText, NULL).x : 0.0f;
    if (ShortcutWidth > 0.0f)
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ShortcutWidth);
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        ImGui::Text(ShortcutText);
        ImGui::PopStyleColor();
    }

    ImGui::Separator();

    bool bShareGamepad = ImGuiWidget->GetShareGamepad();
    if (ImGui::Checkbox("Share Gamepad", &bShareGamepad))
    {
        ImGuiWidget->SetShareGamepad(bShareGamepad);
    }

    bool bShareKeyboard = ImGuiWidget->GetShareKeyboard();
    if (ImGui::Checkbox("Share Keyboard", &bShareKeyboard))
    {
        ImGuiWidget->SetShareKeyboard(bShareKeyboard);
    }

    bool bShareMouse = ImGuiWidget->GetShareMouse();
    if (ImGui::Checkbox("Share Mouse", &bShareMouse))
    {
        //ImGuiWidget->SetShareMouse(bShareMouse);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Mouse sharing is currently not supported");
    }
}
