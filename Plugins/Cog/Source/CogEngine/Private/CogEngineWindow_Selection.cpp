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
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "HAL/IConsoleManager.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"

FString FCogEngineWindow_Selection::ToggleSelectionModeCommand = TEXT("Cog.ToggleSelectionMode");

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
    bHasWidget = true;
    ActorClasses = { AActor::StaticClass(), ACharacter::StaticClass() };

    Config = GetConfig<UCogEngineConfig_Selection>();

    FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
        *ToggleSelectionModeCommand,
        TEXT("Toggle the actor selection mode"),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            ToggleSelectionMode();
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
void FCogEngineWindow_Selection::ToggleSelectionMode()
{
    if (bSelectionModeActive)
    {
        DeactivateSelectionMode();
    }
    else
    {
        ActivateSelectionMode();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::ActivateSelectionMode()
{
    bSelectionModeActive = true;
    bIsInputEnabledBeforeEnteringSelectionMode = GetOwner()->GetContext().GetEnableInput();
    GetOwner()->GetContext().SetEnableInput(true);
    GetOwner()->SetActivateSelectionMode(true);
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogEngineWindow_Selection::HackWaitInputRelease()
{
    WaitInputReleased = 1;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::DeactivateSelectionMode()
{
    bSelectionModeActive = false;

    //--------------------------------------------------------------------------------------------
    // We can enter selection mode by a command, and ImGui might not have the input focus
    // When in selection mode we need ImGui to have the input focus
    // When leaving selection mode we want to leave it as it was before
    //--------------------------------------------------------------------------------------------
    GetOwner()->GetContext().SetEnableInput(bIsInputEnabledBeforeEnteringSelectionMode);

    GetOwner()->SetActivateSelectionMode(false);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (FCogDebug::GetSelection() == nullptr)
    {
        SetGlobalSelection(GetLocalPlayerPawn());
    }

    if (bSelectionModeActive)
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
            ActivateSelectionMode();
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
        DeactivateSelectionMode();
        return;
    }

    APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        DeactivateSelectionMode();
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

    if (bSelectionModeActive)
    {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (WaitInputReleased == 0)
            {
                if (HoveredActor != nullptr)
                {
                    SetGlobalSelection(HoveredActor);
                }

                DeactivateSelectionMode();
            }
            else
            {
                WaitInputReleased--;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogEngineWindow_Selection::GetMainMenuWidgetWidth(int32 SubWidgetIndex, float MaxWidth)
{
    switch (SubWidgetIndex)
    {
        case 0: return FCogWindowWidgets::GetFontWidth() * 6;
        case 1: return FMath::Min(FMath::Max(MaxWidth, FCogWindowWidgets::GetFontWidth() * 10), FCogWindowWidgets::GetFontWidth() * 30);
        case 2: return FCogWindowWidgets::GetFontWidth() * 3;
    }

    return -1.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderMainMenuWidget(int32 SubWidgetIndex, float Width)
{
    //-----------------------------------
    // Pick Button
    //-----------------------------------
    if (SubWidgetIndex == 0)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

        if (ImGui::Button("Pick", ImVec2(Width, 0)))
        {
            ActivateSelectionMode();
            HackWaitInputRelease();
        }
        RenderPickButtonTooltip();

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(2);
    }
    else if (SubWidgetIndex == 1)
    {
        ImGui::SetNextItemWidth(Width);
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
    else if (SubWidgetIndex == 2)
    {
        //-----------------------------------
        // Reset Button
        //-----------------------------------
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
            if (ImGui::Button("X", ImVec2(Width, 0)))
            {
                SetGlobalSelection(nullptr);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Reset the selection to the controlled actor.");
            }
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(1);
        }
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
        const FString Shortcut = FCogImguiInputHelper::CommandToString(*GetWorld(), ToggleSelectionModeCommand);
        ImGui::SetTooltip("Enter picking mode to pick an actor on screen. %s", TCHAR_TO_ANSI(*Shortcut));
    }
}