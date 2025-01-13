#include "CogWindow.h"

#include "CogDebug.h"
#include "CogWindow_Settings.h"
#include "CogWindowManager.h"
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

    if (bHasMenu && bShowMenu)
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

        if (ImGui::BeginPopupContextWindow())
        {
            if (bHasMenu)
            {
                ImGui::Checkbox("Show Menu", &bShowMenu);
            }

            if (ImGui::Button("Reset Settings"))
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
    SetSelection(FCogDebug::GetSelection());
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

void FCogWindow::SetIsVisible(const bool Value)
{
    if (bIsVisible == Value)
    { return; }

    bIsVisible = Value;
    OnWindowVisibilityChanged(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
APawn* FCogWindow::GetLocalPlayerPawn() const
{
	const APlayerController* LocalPlayerController = GetLocalPlayerController();
    
    if (LocalPlayerController == nullptr)
    {
        return nullptr;
    }

    APawn* Pawn = LocalPlayerController->GetPawn();

    return Pawn;
}

//--------------------------------------------------------------------------------------------------------------------------
APlayerController* FCogWindow::GetLocalPlayerController() const
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (LocalPlayer == nullptr)
    {
        return nullptr;
    }

    return LocalPlayer->PlayerController;
}

//--------------------------------------------------------------------------------------------------------------------------
ULocalPlayer* FCogWindow::GetLocalPlayer() const
{
    const UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return nullptr;
    }

    return World->GetFirstLocalPlayerFromController();
}

//--------------------------------------------------------------------------------------------------------------------------
UCogCommonConfig* FCogWindow::GetConfig(const TSubclassOf<UCogCommonConfig> ConfigClass) const
{
    return GetOwner()->GetConfig(ConfigClass);
}

//--------------------------------------------------------------------------------------------------------------------------
const UObject* FCogWindow::GetAsset(const TSubclassOf<UObject> AssetClass) const
{
    return GetOwner()->GetAsset(AssetClass);
}

//--------------------------------------------------------------------------------------------------------------------------
UWorld* FCogWindow::GetWorld() const
{
    return Owner->GetWorld();
}
