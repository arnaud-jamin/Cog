#include "CogEngineWindow_Selection.h"

#include "CogDebug.h"
#include "CogEngineHelper.h"
#include "CogEngineReplicator.h"
#include "CogEngineWindow_ImGui.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogConsoleCommandManager.h"
#include "CogSubsystem.h"
#include "CogWidgets.h"
#include "CogWindow_Settings.h"
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
    bIsWidgetVisible = true;

    Config = GetConfig<UCogEngineConfig_Selection>();

    GetOwner()->AddShortcut(Config.Get(), &UCogEngineConfig_Selection::Shortcut_ToggleSelection, FSimpleDelegate::CreateLambda([this] (){ GetOwner()->SetActivateSelectionMode(!GetOwner()->GetActivateSelectionMode()); }));

    Asset = GetAsset<UCogEngineDataAsset>();

    FCogConsoleCommandManager::RegisterWorldConsoleCommand(
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
void FCogEngineWindow_Selection::PreSaveConfig()
{
    Super::PreSaveConfig();

    if (Config == nullptr)
    { return; }
    
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
    const TArray<TSubclassOf<AActor>>& SelectionFilters = GetSelectionFilters();
    if (SelectionFilters.IsValidIndex(Config->SelectedClassIndex))
    {
        SelectedClass = SelectionFilters[Config->SelectedClassIndex];
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

    if (GetSelection() == nullptr)
    {
        SetGlobalSelection(GetLocalPlayerPawn());
    }

    if (GetOwner()->GetActivateSelectionMode())
    {
        if (TickSelectionMode() == false)
        {
            GetOwner()->SetActivateSelectionMode(false);
        }
    }

    if (const AActor* Actor = GetSelection())
    {
        if (Actor != GetLocalPlayerPawn())
        {
            FCogWidgets::ActorFrame(*Actor);
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
    const bool result = FCogWidgets::ActorsListWithFilters(NewSelection, *GetWorld(), GetSelectionFilters(), Config->SelectedClassIndex, &Filter, GetLocalPlayerPawn());
    if (result)
    {
        SetGlobalSelection(NewSelection);
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Selection::TickSelectionMode()
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    { return false; }

    APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {  return false; }

    ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    { return false; }

    const ImVec2 ViewportPos = Viewport->Pos;
    const ImVec2 ViewportSize = Viewport->Size;
    ImDrawList* DrawList = ImGui::GetBackgroundDrawList(Viewport);
    DrawList->AddRect(ViewportPos, ViewportPos + ViewportSize, IM_COL32(255, 0, 0, 128), 0.0f, 0, 20.0f);
    FCogWidgets::AddTextWithShadow(DrawList, ViewportPos + ImVec2(20, 20), IM_COL32(255, 255, 255, 255), "Picking Mode. \n[LMB] Pick \n[RMB] Cancel");

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
        IgnoreList.Add(GetSelection());

        FHitResult HitResult;
        for (int i = 0; i < 2; ++i)
        {
            if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), WorldOrigin, WorldOrigin + WorldDirection * 10000, GetSelectionTraceChannel(), false, IgnoreList, EDrawDebugTrace::None, HitResult, true))
            {
                AActor* HitActor = HitResult.GetActor();
                if (HitActor != nullptr)
                {
                    if (SelectedActorClass == nullptr || HitActor->GetClass()->IsChildOf(SelectedActorClass))
                    {
                        HoveredActor = HitActor;
                        break;
                    }

                    //------------------------------------------------
                    // The second time we accept the selected actor
                    //------------------------------------------------
                    IgnoreList.Empty();
                }
            }
        }
    }

    if (HoveredActor != nullptr)
    {
        FCogWidgets::ActorFrame(*HoveredActor);
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

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderMainMenuWidget()
{
    ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    if (FCogWidgets::PickButton("##Pick", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight())))
    {
        GetOwner()->SetActivateSelectionMode(true);
        HackWaitInputRelease();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    
    RenderPickButtonTooltip();
    
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    AActor* NewSelection = nullptr;

    //TODO: Could be replaced by a BeginMenu
    if (FCogWidgets::MenuActorsCombo(
        "MenuActorSelection", 
        NewSelection, 
        *GetWorld(), 
        GetSelectionFilters(), 
        Config->SelectedClassIndex, 
        &Filter, 
        GetLocalPlayerPawn(), 
        [this](AActor& Actor) { RenderActorContextMenu(Actor); }))
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
    FCogDebug::SetSelection(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderPickButtonTooltip()
{
    if (FCogWidgets::BeginItemTooltipWrappedText())
    {
        ImGui::Text("Enter selection mode to select an actor on screen. Change which actor type is selectable by clicking the selection combobox\n");
        ImGui::Spacing();
        ImGui::Separator();
        FCogWidgets::TextOfAllInputChordsOfConfig(*Config.Get());
        
        FCogWidgets::EndItemTooltipWrappedText();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
const TArray<TSubclassOf<AActor>>& FCogEngineWindow_Selection::GetSelectionFilters() const
{
    if (Asset != nullptr)
    { return Asset->SelectionFilters; }

    static TArray<TSubclassOf<AActor>> SelectionFilters = { ACharacter::StaticClass(), AActor::StaticClass(), AGameModeBase::StaticClass(), AGameStateBase::StaticClass() };
    return SelectionFilters;
}

//--------------------------------------------------------------------------------------------------------------------------
 ETraceTypeQuery FCogEngineWindow_Selection::GetSelectionTraceChannel() const
{
    if (Asset != nullptr)
    { return Asset->SelectionTraceChannel; }

    return UEngineTypes::ConvertToTraceType(ECC_Pawn);
}
