#include "CogEngineWindow_Selection.h"

#include "CogDebug.h"
#include "CogEngineHelper.h"
#include "CogEngineReplicator.h"
#include "CogEngineWindow_ImGui.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogWindowConsoleCommandManager.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "CogWindow_Settings.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "HAL/IConsoleManager.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Kismet/GameplayStatics.h"

FString FCogEngineWindow_Selection::ToggleSelectionModeCommand = TEXT("Cog.ToggleSelectionMode");

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
    bHasWidget = true;
    bIsWidgetVisible = true;
    ActorClasses = { AActor::StaticClass(), ACharacter::StaticClass() };

    Config = GetConfig<UCogEngineConfig_Selection>();

    FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
        *ToggleSelectionModeCommand,
        TEXT("Toggle the actor selection mode"),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            GetOwner()->SetActivateSelectionMode(!GetOwner()->GetActivateSelectionMode());
        }));

    TryReapplySelection();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderHelp()
{
    ImGui::Text(
        "This window can be used to select an actor either by picking an actor in the world, "
        "or by selecting an actor in the actor list. "
        "The actor list can be filtered by actor type (Actor, Character, etc). "
        "The current selection is used by various debug windows to filter out their content"
        );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::Shutdown()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::PreSaveConfig()
{
    Super::PreSaveConfig();

    Config->SelectionName = GetNameSafe(GetSelection());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::TryReapplySelection() const
{
    const UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return;
    }

    if (Config->bReapplySelection == false)
    {
        return;
    }

    if (Config->SelectionName.IsEmpty())
    {
        return;
    }

    const TSubclassOf<AActor> SelectedClass = GetSelectedActorClass();
    if (SelectedClass == nullptr)
    {
        return;
    }

    TArray<AActor*> Actors;
    for (TActorIterator It(World, SelectedClass); It; ++It)
    {
        AActor* Actor = *It;
        if (GetNameSafe(Actor) == Config->SelectionName)
        {
            SetGlobalSelection(Actor);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
TSubclassOf<AActor> FCogEngineWindow_Selection::GetSelectedActorClass() const
{
    TSubclassOf<AActor> SelectedClass = AActor::StaticClass();
    if (ActorClasses.IsValidIndex(Config->SelectedClassIndex))
    {
        SelectedClass = ActorClasses[Config->SelectedClassIndex];
    }

    return SelectedClass;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::HackWaitInputRelease()
{
    WaitInputReleased = 1;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (FCogDebug::GetSelection() == nullptr)
    {
        SetGlobalSelection(GetLocalPlayerPawn());
    }

    if (GetOwner()->GetActivateSelectionMode())
    {
        TickSelectionMode();
    }

    if (const AActor* Actor = GetSelection())
    {
        if (Actor != GetLocalPlayerPawn())
        {
            FCogWindowWidgets::ActorFrame(*Actor);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Pick"))
        {
            GetOwner()->SetActivateSelectionMode(true);
            //HackWaitInputRelease();
        }

        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Save selection", &Config->bReapplySelection);
            ImGui::SetItemTooltip("Should the last selection be saved and reapplied on startup.");

            ImGui::Checkbox("Actor Name Use Label", &FCogDebug::Settings.ActorNameUseLabel);
            ImGui::SetItemTooltip("Should actor names be displayed using their label. Labels are more readable.");

            ImGui::Checkbox("Replicate Selection", &FCogDebug::Settings.ReplicateSelection);
            ImGui::SetItemTooltip("Should the client replicate its actor selection to the server.");

            ImGui::EndMenu();
        }

        RenderPickButtonTooltip();

        ImGui::EndMenuBar();
    }

    DrawSelectionCombo();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Selection::DrawSelectionCombo()
{
    AActor* NewSelection = nullptr;
    const bool result = FCogWindowWidgets::ActorsListWithFilters(NewSelection, *GetWorld(), ActorClasses, Config->SelectedClassIndex, &Filter, GetLocalPlayerPawn());
    if (result)
    {
        SetGlobalSelection(NewSelection);
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::TickSelectionMode()
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        GetOwner()->SetActivateSelectionMode(false);
        return;
    }

    APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        GetOwner()->SetActivateSelectionMode(false);
        return;
    }

    ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    {
        return;
    }

    const ImVec2 ViewportPos = Viewport->Pos;
    const ImVec2 ViewportSize = Viewport->Size;
    ImDrawList* DrawList = ImGui::GetBackgroundDrawList(Viewport);
    DrawList->AddRect(ViewportPos, ViewportPos + ViewportSize, IM_COL32(255, 0, 0, 128), 0.0f, 0, 20.0f);
    FCogWindowWidgets::AddTextWithShadow(DrawList, ViewportPos + ImVec2(20, 20), IM_COL32(255, 255, 255, 255), "Picking Mode. \n[LMB] Pick \n[RMB] Cancel");

    TSubclassOf<AActor> SelectedActorClass = GetSelectedActorClass();

    AActor* HoveredActor = nullptr;
    FVector WorldOrigin, WorldDirection;
    
    //-----------------------------------------------------------------------------------------------
    // Do not use imgui mouse pos because when connected to NetImgui, the mouse position is invalid.
    // See https://github.com/sammyfreg/netImgui/issues/61
    //-----------------------------------------------------------------------------------------------
    //ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 mousePos = GetOwner()->GetContext().GetImguiMousePos();

    if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, FCogImguiHelper::ToFVector2D(mousePos - ViewportPos), WorldOrigin, WorldDirection))
    {
        //--------------------------------------------------------------------------------------------------------
        // Prioritize another actor than the selected actor unless we only touch the selected actor.
        //--------------------------------------------------------------------------------------------------------
        TArray<AActor*> IgnoreList;
        IgnoreList.Add(FCogDebug::GetSelection());

        FHitResult HitResult;
        for (int i = 0; i < 2; ++i)
        {
            if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), WorldOrigin, WorldOrigin + WorldDirection * 10000, TraceType, false, IgnoreList, EDrawDebugTrace::None, HitResult, true))
            {
                if (SelectedActorClass == nullptr || HitResult.GetActor()->GetClass()->IsChildOf(SelectedActorClass))
                {
                    HoveredActor = HitResult.GetActor();
                    break;
                }

                //------------------------------------------------
                // The second time we accept the selected actor
                //------------------------------------------------
                IgnoreList.Empty();
            }
        }
    }

    if (HoveredActor != nullptr)
    {
        FCogWindowWidgets::ActorFrame(*HoveredActor);
    }

    if (GetOwner()->GetActivateSelectionMode())
    {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (WaitInputReleased == 0)
            {
                if (HoveredActor != nullptr)
                {
                    SetGlobalSelection(HoveredActor);
                }

                GetOwner()->SetActivateSelectionMode(false);
            }
            else
            {
                WaitInputReleased--;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderMainMenuWidget()
{
    if (ImGui::MenuItem("Pick"))
    {
        GetOwner()->SetActivateSelectionMode(true);
        HackWaitInputRelease();
    }
    RenderPickButtonTooltip();

    //TODO: Could be replaced by a BeginMenu
    
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    AActor* NewSelection = nullptr;
    if (FCogWindowWidgets::MenuActorsCombo(
        "MenuActorSelection", 
        NewSelection, 
        *GetWorld(), 
        ActorClasses, 
        Config->SelectedClassIndex, 
        &Filter, 
        GetLocalPlayerPawn(), 
        [this](AActor& Actor) { RenderActorContextMenu(Actor);  }))
    {
        SetGlobalSelection(NewSelection);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderActorContextMenu(AActor& Actor)
{
    FCogEngineHelper::ActorContextMenu(Actor);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::SetGlobalSelection(AActor* Value) const
{
    FCogDebug::SetSelection(GetWorld(), Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderPickButtonTooltip()
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        const FString Shortcut = FCogImguiInputHelper::KeyInfoToString(GetOwner()->GetSettings()->ToggleSelectionShortcut);
        ImGui::SetTooltip("Enter picking mode to pick an actor on screen. %s", TCHAR_TO_ANSI(*Shortcut));
    }
}