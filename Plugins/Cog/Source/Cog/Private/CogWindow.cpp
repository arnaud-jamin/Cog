#include "CogWindow.h"

#include "CogDebug.h"
#include "CogWindow_Settings.h"
#include "CogSubsystem.h"
#include "CogWidgets.h"
#include "Engine/World.h"
#include "imgui_internal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::Initialize()
{
    ensure(bIsInitialized == false);
    bIsInitialized = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::Shutdown()
{
    bIsInitialized = false;
}

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
 void FCogWindow::RenderMainMenuWidget()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::RenderContextMenu()
{
    RenderSettings();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::RenderSettings()
{
    if (bHasMenu)
    {
        ImGui::Checkbox("Show Menu", &bShowMenu);
    }

    if (ImGui::Button("Reset Settings", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
        ResetConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::Render(float DeltaTime)
{
    ImGuiWindowFlags WindowFlags = 0;

    if (bHasMenu && bShowMenu)
    {
        WindowFlags |= ImGuiWindowFlags_MenuBar;
    }

    PreBegin(WindowFlags);
    
    const FString WindowTitle = GetTitle() + "##" + Name;
    const bool IsOpen = ImGui::Begin(StringCast<ANSICHAR>(*WindowTitle).Get(), &bIsVisible, WindowFlags);

    PostBegin();
    
    if (IsOpen)
    {
        RenderContent();

        if (bUseCustomContextMenu == false)
        {
            if (ImGui::BeginPopupContextWindow())
            {
                RenderContextMenu();
                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();
    
    PostEnd();
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
    AActor* OldActor = GetSelection();
    FCogDebug::SetSelection(NewSelection);

    OnSelectionChanged(OldActor, NewSelection);
}

//--------------------------------------------------------------------------------------------------------------------------
AActor* FCogWindow::GetSelection() const
{
    return FCogDebug::GetSelection();
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
UCogCommonConfig* FCogWindow::GetConfig(const TSubclassOf<UCogCommonConfig>& InConfigClass, bool InResetConfigOnRequest) const
{
    UCogCommonConfig* Config = GetOwner()->GetConfig(InConfigClass);

    if (Config != nullptr && InResetConfigOnRequest)
    {
        ConfigsToResetOnRequest.AddUnique(Config);
    }

    return Config;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::ResetConfig()
{
    for (auto& Config : ConfigsToResetOnRequest)
    {
        if (Config != nullptr)
        {
            Config->Reset();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
const UObject* FCogWindow::GetAsset(const TSubclassOf<UObject>& AssetClass) const
{
    return GetOwner()->GetAsset(AssetClass);
}

//--------------------------------------------------------------------------------------------------------------------------
UWorld* FCogWindow::GetWorld() const
{
    return Owner->GetWorld();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindow::IsWindowRenderedInMainMenu()
{
    return Owner->IsRenderingMainMenu();
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogWindow::GetDpiScale() const
{
    return GetOwner()->GetContext().GetDpiScale();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow::RenderConfigShortcuts(UCogCommonConfig& InConfig) const
{
    FProperty* InModifiedProperty = nullptr;
    if (FCogWidgets::AllInputChordsOfConfig(InConfig, &InModifiedProperty))
    {
        GetOwner()->RebindShortcut(InConfig, *InModifiedProperty);
    }
}