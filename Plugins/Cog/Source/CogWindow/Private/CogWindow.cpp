#include "CogWindow.h"

#include "CogDebugDraw.h"
#include "CogDebugSettings.h"
#include "CogWindow_Settings.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "Engine/World.h"
#include "imgui_internal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::SetFullName(const FString& InFullName)
{
    FullName = InFullName;

    TArray<FString> Path;
    int CharIndex = 0;
    if (FullName.FindLastChar(TEXT('.'), CharIndex))
    {
        Name = FullName.RightChop(CharIndex + 1);
    }
    else
    {
        Name = FullName;
    }

    Title = Name;

    ID = ImHashStr(TCHAR_TO_ANSI(*FullName));
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindow::CheckEditorVisibility()
{
    const UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return false;
    }

    if (World->WorldType != EWorldType::Game)
    {   
        return false;
    }
    
    if (World->WorldType == EWorldType::PIE)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::Render(float DeltaTime)
{
    ImGuiWindowFlags WindowFlags = 0;
    PreRender(WindowFlags);

    const FString WindowTitle = GetTitle() + "##" + Name;

    if (bHasMenu && bHideMenu == false)
    {
        WindowFlags |= ImGuiWindowFlags_MenuBar;
    }

    if (bNoPadding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    }

    if (ImGui::Begin(TCHAR_TO_ANSI(*WindowTitle), &bIsVisible, WindowFlags))
    {
        if (bNoPadding)
        {
            ImGui::PopStyleVar(1);
        }

        if (GetOwner()->GetSettingsWindow()->GetSettingsConfig()->bShowHelp)
        {
            if (ImGui::IsItemHovered())
            {
                ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(29, 42, 62, 240));
                const float HelpWidth = FCogWindowWidgets::GetFontWidth() * 80;
                ImGui::SetNextWindowSizeConstraints(ImVec2(HelpWidth / 2.0f, 0.0f), 
                                                    ImVec2(HelpWidth, FLT_MAX));
                if (ImGui::BeginTooltip())
                {
                    ImGui::PushTextWrapPos(HelpWidth - 1 * FCogWindowWidgets::GetFontWidth());
                    RenderHelp();
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::PopStyleColor();
            }
        }

        if (ImGui::BeginPopupContextWindow())
        {
            if (bHasMenu)
            {
                ImGui::Checkbox("Hide Menu", &bHideMenu);
            }

            if (ImGui::Button("Reset"))
            {
                ResetConfig();
            }

            ImGui::EndPopup();
        }

        RenderContent();
    }
    else
    {
        if (bNoPadding)
        {
            ImGui::PopStyleVar(1);
        }
    }

    ImGui::End();

    PostRender();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::RenderHelp()
{
    ImGui::Text("No help available.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::RenderTick(float DeltaTime)
{
    SetSelection(FCogDebugSettings::GetSelection());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::GameTick(float DeltaTime)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::SetSelection(AActor* NewSelection)
{
    if (CurrentSelection == NewSelection)
    {
        return;
    }

    AActor* OldActor = CurrentSelection.Get();

    CurrentSelection = NewSelection;
    OnSelectionChanged(OldActor, NewSelection);
}

//--------------------------------------------------------------------------------------------------------------------------
APawn* FCogWindow::GetLocalPlayerPawn()
{
    APlayerController* LocalPlayerController = GetLocalPlayerController();
    
    if (LocalPlayerController == nullptr)
    {
        return nullptr;
    }

    APawn* Pawn = LocalPlayerController->GetPawn();

    return Pawn;
}

//--------------------------------------------------------------------------------------------------------------------------
APlayerController* FCogWindow::GetLocalPlayerController()
{
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (LocalPlayer == nullptr)
    {
        return nullptr;
    }

    return LocalPlayer->PlayerController;
}

//--------------------------------------------------------------------------------------------------------------------------
ULocalPlayer* FCogWindow::GetLocalPlayer()
{
    const UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return nullptr;
    }

    return World->GetFirstLocalPlayerFromController();
}

//--------------------------------------------------------------------------------------------------------------------------
UCogWindowConfig* FCogWindow::GetConfig(const TSubclassOf<UCogWindowConfig> ConfigClass)
{
    return GetOwner()->GetConfig(ConfigClass);
}

//--------------------------------------------------------------------------------------------------------------------------
const UObject* FCogWindow::GetAsset(const TSubclassOf<UObject> AssetClass)
{
    return GetOwner()->GetAsset(AssetClass);
}

//--------------------------------------------------------------------------------------------------------------------------
UWorld* FCogWindow::GetWorld() const
{
    return Owner->GetWorld();
}

