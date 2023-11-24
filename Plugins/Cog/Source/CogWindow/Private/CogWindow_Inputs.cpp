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

    Config = GetConfig<UCogEngineConfig_Inputs>();

    SCogImguiWidget* ImGuiWidget = GetOwner()->GetImGuiWidget().Get();
    ImGuiWidget->SetEnableInput(Config->bEnableInput);

    ImGuiIO& IO = ImGui::GetIO();
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard, Config->bNavEnableKeyboard);
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad, Config->bNavEnableGamepad);
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavNoCaptureKeyboard, Config->bNavNoCaptureInput);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Inputs::PreSaveConfig()
{
    Super::PreSaveConfig();

    ImGuiIO& IO = ImGui::GetIO();
    Config->bNavEnableKeyboard = IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard;
    Config->bNavEnableGamepad = IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad;
    Config->bNavNoCaptureInput = IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard;

    if (SCogImguiWidget* ImGuiWidget = GetOwner()->GetImGuiWidget().Get())
    {
        Config->bEnableInput = ImGuiWidget->GetEnableInput();
        Config->bShareMouse = ImGuiWidget->GetShareMouse();
    }
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
    
    bool bShareMouse = ImGuiWidget->GetShareMouse();
    if (ImGui::Checkbox("Share Mouse", &bShareMouse))
    {
        ImGuiWidget->SetShareMouse(bShareMouse);
    }

    ImGuiIO& IO = ImGui::GetIO();
    ImGui::CheckboxFlags("Keyboard Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
    ImGui::CheckboxFlags("Gamepad Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
    ImGui::CheckboxFlags("Navigation No Capture", &IO.ConfigFlags, ImGuiConfigFlags_NavNoCaptureKeyboard);
}
