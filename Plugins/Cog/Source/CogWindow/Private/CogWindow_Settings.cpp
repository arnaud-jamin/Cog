#include "CogWindow_Settings.h"

#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
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
    FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard, Config->bNavEnableKeyboard);
    //FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad, Config->bNavEnableGamepad);
    //FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_NavNoCaptureKeyboard, Config->bNavNoCaptureInput);

    GetOwner()->GetContext().SetDPIScale(Config->DPIScale);

    FCogImguiContext& Context = GetOwner()->GetContext();
    Context.SetEnableInput(Config->bEnableInput);
    Context.SetShareKeyboard(Config->bShareKeyboard);
    Context.SetShareMouse(Config->bShareMouse);
    Context.SetShowCursorWhenSharingMouse(Config->bShowCursorWhenSharingMouse);


}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::PreSaveConfig()
{
    Super::PreSaveConfig();

    ImGuiIO& IO = ImGui::GetIO();
    Config->bNavEnableKeyboard = IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard;
    //Config->bNavEnableGamepad = IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad;
    //Config->bNavNoCaptureInput = IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard;

    const FCogImguiContext& Context = GetOwner()->GetContext();
    Config->bEnableInput = Context.GetEnableInput();
    Config->bShareKeyboard = Context.GetShareKeyboard();
    Config->bShareMouse = Context.GetShareMouse();
    Config->bShowCursorWhenSharingMouse = Context.GetShowCursorWhenSharingMouse();
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

    //-----------------------------
    // bEnableInput
    //-----------------------------
    bool bEnableInput = Context.GetEnableInput();
    if (ImGui::Checkbox("Enable Input", &bEnableInput))
    {
        Context.SetEnableInput(bEnableInput);
    }
    ImGui::SetItemTooltip("Enable ImGui inputs. When enabled the ImGui menu is shown and inputs are forwarded to ImGui.");

    const auto ShortcutText = StringCast<ANSICHAR>(*FCogImguiInputHelper::CommandToString(PlayerInput, UCogWindowManager::ToggleInputCommand));
    const float ShortcutWidth = (ShortcutText.Get() != nullptr && ShortcutText.Get()[0]) ? ImGui::CalcTextSize(ShortcutText.Get(), NULL).x : 0.0f;
    if (ShortcutWidth > 0.0f)
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ShortcutWidth);
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        ImGui::Text("%s", ShortcutText.Get());
        ImGui::PopStyleColor();
    }

    //-----------------------------
    // ShareKeyboard
    //-----------------------------
    bool bShareKeyboard = Context.GetShareKeyboard();
    if (ImGui::Checkbox("Share Keyboard", &bShareKeyboard))
    {
        Context.SetShareKeyboard(bShareKeyboard);
    }
    ImGui::SetItemTooltip("Forward the keyboard inputs to the game when ImGui does not need them.");

    //-----------------------------
    // ShareMouse
    //-----------------------------
    bool bShareMouse = Context.GetShareMouse();
    if (ImGui::Checkbox("Share Mouse", &bShareMouse))
    {
        Context.SetShareMouse(bShareMouse);
    }
    ImGui::SetItemTooltip("Forward mouse inputs to the game when ImGui does not need them.");

    //-----------------------------
    // ShowCursorWhenSharingMouse
    //-----------------------------
    if (bShareMouse == false)
    {
        ImGui::BeginDisabled();
    }

    bool bShowCursorWhenSharingMouse = Context.GetShowCursorWhenSharingMouse();
    if (ImGui::Checkbox("Show Cursor When Sharing Mouse", &bShowCursorWhenSharingMouse))
    {
        Context.SetShowCursorWhenSharingMouse(bShowCursorWhenSharingMouse);
    }
    ImGui::SetItemTooltip("Show the mouse cursor when ImGui share the mouse with the game. Useful for games that require the cursor to be hidden.");

    if (bShareMouse == false)
    {
        ImGui::EndDisabled();
    }

    //-----------------------------
    // NavEnableKeyboard
    //-----------------------------
    ImGui::CheckboxFlags("Keyboard Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
    ImGui::SetItemTooltip("Use the keyboard to navigate in ImGui windows with the following keys : Tab, Directional Arrows, Space, Enter.");

    //-----------------------------
    // NavEnableGamepad
    //-----------------------------
    //ImGui::CheckboxFlags("Gamepad Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
    //ImGui::SetItemTooltip("Use the gamepad to navigate in ImGui windows.");

    ImGui::Separator();

    //-----------------------------
    // DPI
    //-----------------------------
    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat("DPI Scale", &Config->DPIScale, 0.5f, 2.0f, "%.1f");
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        SetDPIScale(Config->DPIScale);
    }
    if (ImGui::BeginItemTooltip())
    {
        ImGui::TextUnformatted("Change DPi Scale [Mouse Wheel]");
        ImGui::TextUnformatted("Reset DPi Scale  [Middle Mouse]");
        ImGui::EndTooltip();
    }

    //-----------------------------
    // EnableViewports
    //-----------------------------
    if (ImGui::Checkbox("Enable Viewports", &Config->bEnableViewports))
    {
        FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_ViewportsEnable, Config->bEnableViewports);
    }
    ImGui::SetItemTooltip("Enable moving ImGui windows outside of the main viewport.");

    //-----------------------------
    // CompactMode
    //-----------------------------
    ImGui::Checkbox("Compact Mode", &Config->bCompactMode);
    ImGui::SetItemTooltip("Enable compact mode.");
    
    //-----------------------------
    // ShowWindowsInMainMenu
    //-----------------------------
    ImGui::Checkbox("Show Windows In Main Menu", &Config->bShowWindowsInMainMenu);
    ImGui::SetItemTooltip("Show the content of the windows when hovering the window menu item.");

    //-----------------------------
    // ShowHelp
    //-----------------------------
    ImGui::Checkbox("Show Help", &Config->bShowHelp);
    ImGui::SetItemTooltip("Show windows help on the window menu items.");

    ImGui::Separator();

    if (ImGui::Button("Reset All Windows Config", ImVec2(-1.0f, 0.0f)))
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
void FCogWindow_Settings::SetDPIScale(float Value) const
{
    Config->DPIScale = Value;
    GetOwner()->GetContext().SetDPIScale(Config->DPIScale);
}
