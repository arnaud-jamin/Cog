#include "CogWindow.h"

#include "CogDebugDraw.h"
#include "CogDebugSettings.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "Engine/World.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow::SetFullName(const FString& InFullName)
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

    ID = ImHashStr(TCHAR_TO_ANSI(*FullName));
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogWindow::CheckEditorVisibility()
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
void UCogWindow::Render(float DeltaTime)
{
    ImGuiWindowFlags WindowFlags = 0;
    PreRender(WindowFlags);

    if (ImGui::Begin(TCHAR_TO_ANSI(*GetTitle()), &bIsVisible, WindowFlags))
    {
        RenderContent();
        ImGui::End();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow::RenderTick(float DeltaTime)
{
    SetSelection(FCogDebugSettings::GetSelection());
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow::GameTick(float DeltaTime)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow::DrawMenuItem(const FString& MenuItemName)
{
    if (bShowInsideMenu && bIsVisible == false)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(FCogWindowWidgets::TextBaseWidth * 40, FCogWindowWidgets::TextBaseHeight * 1),
                                            ImVec2(FCogWindowWidgets::TextBaseWidth * 50, FCogWindowWidgets::TextBaseHeight * 60));

        if (ImGui::BeginMenu(TCHAR_TO_ANSI(*MenuItemName)))
        {
            RenderContent();
            ImGui::EndMenu();

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                bIsVisible = !bIsVisible;
            }
        }
    }
    else
    {
        ImGui::MenuItem(TCHAR_TO_ANSI(*MenuItemName), nullptr, &bIsVisible);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindow::SetSelection(AActor* NewSelection)
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
APawn* UCogWindow::GetLocalPlayerPawn()
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
APlayerController* UCogWindow::GetLocalPlayerController()
{
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (LocalPlayer == nullptr)
    {
        return nullptr;
    }

    return LocalPlayer->PlayerController;
}

//--------------------------------------------------------------------------------------------------------------------------
ULocalPlayer* UCogWindow::GetLocalPlayer()
{
    const UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return nullptr;
    }

    return World->GetFirstLocalPlayerFromController();
}
