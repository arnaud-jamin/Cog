#include "CogWindow_Settings.h"

#include "CogCommon.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogSubsystem.h"
#include "CogWidgets.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "InputCoreTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
FCogWindow_Settings::FCogWindow_Settings()
{
    bShowInMainMenu = false;
    bHasMenu = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::Initialize()
{
    Super::Initialize();


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
    Context.SetShareMouseWithGameplay(Config->bShareMouseWithGameplay);
    Context.SetFont(Config->Font);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Settings::PreSaveConfig()
{
    Super::PreSaveConfig();

    ImGuiIO& IO = ImGui::GetIO();

    if (Config == nullptr)
    { return; }

    Config->bNavEnableKeyboard = IO.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard;
    //Config->bNavEnableGamepad = IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad;
    //Config->bNavNoCaptureInput = IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard;

    const FCogImguiContext& Context = GetOwner()->GetContext();
    Config->bEnableInput = Context.GetEnableInput();
    Config->bShareKeyboard = Context.GetShareKeyboard();
    Config->bShareMouse = Context.GetShareMouse();
    Config->bShareMouseWithGameplay = Context.GetShareMouseWithGameplay();
    Config->Font = Context.GetFont();
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

    //-------------------------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Input", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool bEnableInput = Context.GetEnableInput();
        if (ImGui::Checkbox("Enable Input", &bEnableInput))
        {
            Context.SetEnableInput(bEnableInput);
        }
        FCogWidgets::ItemTooltipWrappedText("Enable ImGui inputs. When enabled the ImGui menu is shown and inputs are forwarded to ImGui.");
        FCogWidgets::MenuItemShortcut("EnableInputShortcut", FCogImguiInputHelper::InputChordToString(Config->Shortcut_ToggleImguiInput));

        //-------------------------------------------------------------------------------------------
        bool bShareKeyboard = Context.GetShareKeyboard();
        if (ImGui::Checkbox("Share Keyboard", &bShareKeyboard))
        {
            Context.SetShareKeyboard(bShareKeyboard);
        }
        FCogWidgets::ItemTooltipWrappedText("Forward the keyboard inputs to the game when ImGui does not need them.");

        //-------------------------------------------------------------------------------------------
        bool bShareMouse = Context.GetShareMouse();
        if (ImGui::Checkbox("Share Mouse", &bShareMouse))
        {
            Context.SetShareMouse(bShareMouse);
        }
        FCogWidgets::ItemTooltipWrappedText("Forward mouse inputs to the game when ImGui does not need them.");

        //-------------------------------------------------------------------------------------------
        if (bShareMouse == false)
        {
            ImGui::BeginDisabled();
        }

        bool bShareMouseWithGameplay = Context.GetShareMouseWithGameplay();
        if (ImGui::Checkbox("Share Mouse With Gameplay", &bShareMouseWithGameplay))
        {
            Context.SetShareMouseWithGameplay(bShareMouseWithGameplay);
        }
        FCogWidgets::ItemTooltipWrappedText("When disabled, mouse inputs are only forwarded to game menus. "
        "When enabled, mouse inputs are also forwarded to the gameplay. Note that this mode: \n" 
        "  - Force the cursor to be visible.\n"
        "  - Prevent the interaction of Cog's transform gizmos.\n"
        );

        if (bShareMouse == false)
        {
            ImGui::EndDisabled();
        }

        //-------------------------------------------------------------------------------------------
        ImGui::CheckboxFlags("Keyboard Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
        FCogWidgets::ItemTooltipWrappedText("Use the keyboard to navigate in ImGui windows with the following keys : Tab, Directional Arrows, Space, Enter.");

        //-------------------------------------------------------------------------------------------
        //ImGui::CheckboxFlags("Gamepad Navigation", &IO.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
        //FCogWidgets::ItemTooltipWrappedText("Use the gamepad to navigate in ImGui windows.");

        //-------------------------------------------------------------------------------------------
        ImGui::Checkbox("Disable Conflicting Commands", &Config->bDisableConflictingCommands);
        FCogWidgets::ItemTooltipWrappedText("Disable the existing Unreal command shortcuts mapped to same shortcuts Cog is using. Typically, if the F1 shortcut is used to toggle Inputs, the Unreal wireframe command will get disabled.");

        //-------------------------------------------------------------------------------------------
        ImGui::Checkbox("Disable shortcuts when ImGui want text input", &Config->bDisableShortcutsWhenImGuiWantTextInput);
        FCogWidgets::ItemTooltipWrappedText("Disable Cog's shortcuts (ToggleInput, ToggleSelectionMode, LoadLayout, ...) when ImGui want text input."
				" This can be required if the shortcuts are mapped by keys generating a text input (letters, or Backspace for example)."
                " This is not required if the shortcuts are set to keys such as F1 or F2.");
    }
    
    //-------------------------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Checkbox("Enable Viewports", &Config->bEnableViewports))
        {
            FCogImguiHelper::SetFlags(IO.ConfigFlags, ImGuiConfigFlags_ViewportsEnable, Config->bEnableViewports);
        }
        FCogWidgets::ItemTooltipWrappedText("Enable moving ImGui windows outside of the main viewport.");

        //-------------------------------------------------------------------------------------------
        ImGui::Checkbox("Compact Mode", &Config->bCompactMode);
        FCogWidgets::ItemTooltipWrappedText("Enable compact mode.");

        //-------------------------------------------------------------------------------------------
        ImGui::Checkbox("Transparent Mode", &Config->bTransparentMode);
        FCogWidgets::ItemTooltipWrappedText("Enable transparent mode.");
    
        //-------------------------------------------------------------------------------------------
        ImGui::Checkbox("Show Windows In Main Menu", &Config->bShowWindowsInMainMenu);
        FCogWidgets::ItemTooltipWrappedText("Show the content of the windows when hovering the window menu item.");

        //-------------------------------------------------------------------------------------------
        ImGui::Checkbox("Show Help", &Config->bShowHelp);
        FCogWidgets::ItemTooltipWrappedText("Show windows help on the window menu items.");

        //-------------------------------------------------------------------------------------------
        FCogWidgets::SetNextItemToShortWidth();
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
    }

    //-------------------------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Widgets (?)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        FCogWidgets::ItemTooltipWrappedText("Widgets appear in the main menu bar.");
        
        ImGui::Checkbox("Show Widget Borders", &Config->ShowWidgetBorders);
        FCogWidgets::ItemTooltipWrappedText("Should a border be visible between widgets.");
        
        FCogWidgets::SetNextItemToShortWidth();
        FCogWidgets::ComboboxEnum("Widgets Alignment", Config->WidgetAlignment);
        FCogWidgets::ItemTooltipWrappedText("How the widgets should be aligned in the main menu bar.");

        if (ImGui::BeginChild("Widgets", ImVec2(0, ImGui::GetFontSize() * 10), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                ImGui::TextUnformatted("Widgets visibility and ordering");
                ImGui::SameLine();
                FCogWidgets::HelpMarker("Drag and drop the widget names to reorder them.");
                ImGui::EndMenuBar();
            }
            
            TArray<FCogWindow*>& Widgets = GetOwner()->Widgets;
            for (int32 i = 0; i < Widgets.Num(); ++i)
            {
                FCogWindow* Window = Widgets[i];

                ImGui::PushID(i);

                bool Visible = Window->GetIsWidgetVisible();
                if (ImGui::Checkbox("##Visibility", &Visible))
                {
                    Window->SetIsWidgetVisible(Visible);
                }
                
                ImGui::SameLine();
                ImGui::Selectable(COG_TCHAR_TO_CHAR(*Window->GetName()), false, ImGuiSelectableFlags_SpanAvailWidth);
                {
                    Window->SetIsWidgetVisible(Visible);
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                }

                if (ImGui::IsItemActive() && ImGui::IsItemHovered() == false)
                {
                    const int iNext = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (iNext >= 0 && iNext < Widgets.Num())
                    {
                        Widgets[i] = Widgets[iNext];
                        Widgets[iNext] = Window;
                        ImGui::ResetMouseDragDelta();
                    }
                }

                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }
    
    //-------------------------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen))
    {
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

        FString FontDirectory = FPaths::EngineContentDir() / TEXT("Slate/Fonts");
        if (PlatformFile.DirectoryExists(*FontDirectory))
        {
            static TArray<FString> FontPaths;
            if (FontPaths.IsEmpty())
            {
                PlatformFile.FindFiles(FontPaths, *FontDirectory, TEXT(".ttf"));
            }

            if (ImGui::BeginChild("FontFiles", ImVec2(0, ImGui::GetFontSize() * 10), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
            {
                {
                    bool Selected = Context.GetFont().IsEmpty();
                    if (ImGui::Checkbox("Default", &Selected))
                    {
                        Context.SetFont(FString());
                    }
                }

                ImGui::Separator();

                for (const FString& FontPath : FontPaths)
                {
                    bool Selected = Context.GetFont() == FontPath;
                    FString FontName = FPaths::GetBaseFilename(FontPath, true);
                    if (ImGui::Checkbox(COG_TCHAR_TO_CHAR(*FontName), &Selected))
                    {
                        Context.SetFont(FontPath);
                    }
                }
            }

            ImGui::EndChild();
        }

    }

    //-------------------------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Shortcuts", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (TObjectPtr SomeConfig : GetOwner()->GetConfigs())
        {
            if (SomeConfig == nullptr)
            { continue; }
            
            if (FCogWidgets::IsConfigContainingInputChords(*SomeConfig))
            {
                auto ConfigName = StringCast<ANSICHAR>(*FCogWidgets::FormatConfigName(SomeConfig->GetClass()->GetName()));
                ImGui::SeparatorText(ConfigName.Get());

                FProperty* InModifiedProperty = nullptr;
                if (FCogWidgets::AllInputChordsOfConfig(*SomeConfig, &InModifiedProperty))
                {
                    GetOwner()->RebindShortcut(*SomeConfig, *InModifiedProperty);
                }
            }
        }
    }
    
    //-------------------------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Save All Settings", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            GetOwner()->SaveAllSettings();
        }

        // if (ImGui::Button("Reload All Settings", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        // {
        //     GetOwner()->ReloadAllSettings();
        // }

        FCogWidgets::PushButtonBackColor(ImVec4(1.0f, 0.0f, 0.0f, 1));
        if (ImGui::Button("Reset All Settings", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            GetOwner()->ResetAllWindowsConfig();
        }
        FCogWidgets::PopButtonBackColor();
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
