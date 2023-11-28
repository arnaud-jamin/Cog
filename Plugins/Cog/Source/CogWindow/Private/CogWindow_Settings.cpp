#include "CogWindow_Settings.h"

#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiModule.h"
#include "CogImguiWidget.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "imgui.h"
#include "InputCoreTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::Initialize()
{
    Super::Initialize();

    bHasMenu = false;

    Config = GetConfig<UCogWindowConfig_Settings>();

    ImGuiIO& IO = ImGui::GetIO();
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_ViewportsEnable, Config->bEnableViewports);

    GetOwner()->GetContext().SetDPIScale(Config->DPIScale);

    FCogImguiContext& Context = GetOwner()->GetContext();
    Context.SetEnableInput(Config->bEnableInput);
    Context.SetShareMouse(Config->bShareMouse);

    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard, Config->bNavEnableKeyboard);
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad, Config->bNavEnableGamepad);
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavNoCaptureKeyboard, Config->bNavNoCaptureInput);
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NoMouseCursorChange, Config->bNoMouseCursorChange);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::PreSaveConfig()
{
    Super::PreSaveConfig();

    ImGuiIO& IO = ImGui::GetIO();
    Config->bNavEnableKeyboard = IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard;
    Config->bNavEnableGamepad = IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad;
    Config->bNavNoCaptureInput = IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard;
    Config->bNoMouseCursorChange = IO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange;

    const FCogImguiContext& Context = GetOwner()->GetContext();
    Config->bEnableInput = Context.GetEnableInput();
    Config->bShareMouse = Context.GetShareMouse();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::RenderContent()
{
    const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*GetWorld());
    if (PlayerInput == nullptr)
    {
        return;
    }
    
    ImGuiIO& IO = ImGui::GetIO();

    FCogImguiContext& Context = GetOwner()->GetContext();
    bool bEnableInput = Context.GetEnableInput();
    if (ImGui::Checkbox("Enable Input", &bEnableInput))
    {
        Context.SetEnableInput(bEnableInput);
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

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat("DPI Scale", &Config->DPIScale, 0.5f, 2.0f, "%.1f");
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        SetDPIScale(Config->DPIScale);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Change DPi Scale [Mouse Wheel]");
        ImGui::TextUnformatted("Reset DPi Scale  [Middle Mouse]");
        ImGui::EndTooltip();
    }

    ImGui::Separator();

    if (ImGui::Checkbox("Enable Viewports", &Config->bEnableViewports))
    {
        FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_ViewportsEnable, Config->bEnableViewports);
    }

    ImGui::Checkbox("Compact Mode", &Config->bCompactMode);

    ImGui::Checkbox("Show Windows In Main Menu", &Config->bShowWindowsInMainMenu);

    ImGui::Checkbox("Show Window Help", &Config->bShowHelp);
    
    ImGui::Separator();

    bool bShareMouse = Context.GetShareMouse();
    if (ImGui::Checkbox("Share Mouse", &bShareMouse))
    {
        Context.SetShareMouse(bShareMouse);
    }

    ImGui::CheckboxFlags("Keyboard Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
    ImGui::CheckboxFlags("Gamepad Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
    ImGui::CheckboxFlags("Navigation No Capture", &IO.ConfigFlags, ImGuiConfigFlags_NavNoCaptureKeyboard);
    ImGui::CheckboxFlags("No Mouse Cursor Change", &IO.ConfigFlags, ImGuiConfigFlags_NoMouseCursorChange);

    ImGui::Separator();

    if (ImGui::Button("Reset All Windows Config"))
    {
        GetOwner()->ResetAllWindowsConfig();
    }

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    const float MouseWheel = ImGui::GetIO().MouseWheel;
    const bool IsControlDown = ImGui::GetIO().KeyCtrl;
    if (IsControlDown && MouseWheel != 0)
    {
        SetDPIScale(FMath::Clamp(Config->DPIScale + (MouseWheel > 0 ? 0.1f : -0.1f), 0.5f, 2.0f));
    }

    const bool IsMiddleMouseClicked = ImGui::GetIO().MouseClicked[2];
    if (IsControlDown && IsMiddleMouseClicked)
    {
        SetDPIScale(1.0f);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::SetDPIScale(float Value)
{
    Config->DPIScale = Value;
    GetOwner()->GetContext().SetDPIScale(Config->DPIScale);
}
