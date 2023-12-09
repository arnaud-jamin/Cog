#include "CogEngineWindow_Selection.h"

#include "CogDebugDraw.h"
#include "CogDebugSettings.h"
#include "CogEngineReplicator.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiModule.h"
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

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        *ToggleSelectionModeCommand,
        TEXT("Toggle the actor selection mode"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { ToggleSelectionMode(); }),
        ECVF_Cheat));

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
    for (IConsoleObject* ConsoleCommand : ConsoleCommands)
    {
        IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);
    }
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
    UWorld* World = GetWorld();
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

    TSubclassOf<AActor> SelectedClass = GetSelectedActorClass();
    if (SelectedClass == nullptr)
    {
        return;
    }

    TArray<AActor*> Actors;
    for (TActorIterator<AActor> It(World, SelectedClass); It; ++It)
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
    GetOwner()->SetHideAllWindows(true);
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
    // We can enter selection mode by a command, and imgui might not have the input focus
    // When in selection mode we need imgui to have the input focus
    // When leaving selection mode we want to leave it as is was before
    //--------------------------------------------------------------------------------------------
    GetOwner()->GetContext().SetEnableInput(bIsInputEnabledBeforeEnteringSelectionMode);

    GetOwner()->SetHideAllWindows(false);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (FCogDebugSettings::GetSelection() == nullptr)
    {
        SetGlobalSelection(GetLocalPlayerPawn());
    }

    if (bSelectionModeActive)
    {
        TickSelectionMode();
    }

    if (AActor* Actor = GetSelection())
    {
        if (Actor != GetLocalPlayerPawn())
        {
            DrawActorFrame(*Actor);
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
            HackWaitInputRelease();
        }
        RenderPickButtonTooltip();

        ImGui::EndMenuBar();
    }

    DrawSelectionCombo();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Selection::DrawSelectionCombo()
{
    bool SelectionChanged = false;

    APawn* LocalPlayerPawn = GetLocalPlayerPawn();

    //------------------------
    // Actor Class Combo
    //------------------------

    TSubclassOf<AActor> SelectedClass = GetSelectedActorClass();

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##SelectionType", TCHAR_TO_ANSI(*GetNameSafe(SelectedClass))))
    {
        for (int32 i = 0; i < ActorClasses.Num(); ++i)
        {
            TSubclassOf<AActor> SubClass = ActorClasses[i];
            if (ImGui::Selectable(TCHAR_TO_ANSI(*GetNameSafe(SubClass)), false))
            {
                Config->SelectedClassIndex = i;
                SelectedClass = SubClass;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    //------------------------
    // Actor List
    //------------------------
    ImGui::BeginChild("ActorList", ImVec2(-1, -1), false);

    TArray<AActor*> Actors;
    for (TActorIterator<AActor> It(GetWorld(), SelectedClass); It; ++It)
    {
        AActor* Actor = *It;
        Actors.Add(Actor);
    }

    ImGuiListClipper Clipper;
    Clipper.Begin(Actors.Num());
    while (Clipper.Step())
    {
        for (int32 i = Clipper.DisplayStart; i < Clipper.DisplayEnd; i++)
        {
            AActor* Actor = Actors[i];
            if (Actor == nullptr)
            {
                continue;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, Actor == LocalPlayerPawn ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 255, 255));

            bool bIsSelected = Actor == FCogDebugSettings::GetSelection();
            if (ImGui::Selectable(TCHAR_TO_ANSI(*GetActorName(*Actor)), bIsSelected))
            {
                SetGlobalSelection(Actor);
                SelectionChanged = true;
            }

            ImGui::PopStyleColor(1);

            DrawActorContextMenu(Actor);

            //------------------------
            // Draw Frame
            //------------------------
            if (ImGui::IsItemHovered())
            {
                DrawActorFrame(*Actor);
            }

            if (bIsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
    }
    Clipper.End();
    ImGui::EndChild();

    return SelectionChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogEngineWindow_Selection::GetActorName(const AActor* Actor) const
{
    if (Actor == nullptr)
    {
        return FString("none");
    }

    return GetActorName(*Actor);
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogEngineWindow_Selection::GetActorName(const AActor& Actor) const
{
#if WITH_EDITOR

    return Config->bDisplayActorLabel ? Actor.GetActorLabel() : Actor.GetName();

#else //WITH_EDITOR

    return Actor.GetName();

#endif //WITH_EDITOR
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Selection::DrawActorContextMenu(AActor* Actor)
{
    //------------------------
    // ContextMenu
    //------------------------
    ImGui::SetNextWindowSize(ImVec2(FCogWindowWidgets::GetFontWidth() * 30, 0));
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Button("Reset Selection", ImVec2(-1, 0)))
        {
            ImGui::CloseCurrentPopup();
            SetGlobalSelection(GetLocalPlayerPawn());
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Reset the selection to the controlled actor.");
        }

        if (APawn* Pawn = Cast<APawn>(Actor))
        {
            if (ImGui::Button("Possess", ImVec2(-1, 0)))
            {
                if (ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld()))
                {
                    Replicator->Server_Possess(Pawn);
                }

            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Possess this pawn.");
            }

            if (ImGui::Button("Reset Possession", ImVec2(-1, 0)))
            {
                if (ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld()))
                {
                    Replicator->Server_ResetPossession();
                }

            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Reset pawn.");
            }
        }

        ImGui::Separator();

        ImGui::Checkbox("Save selection", &Config->bReapplySelection);
        ImGui::Checkbox("Display Actor Label", &Config->bDisplayActorLabel);

        ImGui::EndPopup();
    }
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
    if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, FCogImguiHelper::ToFVector2D(ImGui::GetMousePos() - ViewportPos), WorldOrigin, WorldDirection))
    {
        //--------------------------------------------------------------------------------------------------------
        // Prioritize another actor than the selected actor unless we only touch the selected actor.
        //--------------------------------------------------------------------------------------------------------
        TArray<AActor*> IgnoreList;
        IgnoreList.Add(FCogDebugSettings::GetSelection());

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
        DrawActorFrame(*HoveredActor);
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
void FCogEngineWindow_Selection::DrawActorFrame(const AActor& Actor)
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    {
        return;
    }

    ImDrawList* DrawList = ImGui::GetBackgroundDrawList(Viewport);

    FVector BoxOrigin, BoxExtent;

    bool PrimitiveFound = false;
    FBox Bounds(ForceInit);
    
    if (const UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Actor.GetRootComponent()))
    {
        PrimitiveFound = true;
    
        Bounds += PrimitiveComponent->Bounds.GetBox();
    }
    else
    {
        Actor.ForEachComponent<UPrimitiveComponent>(true, [&](const UPrimitiveComponent* InPrimComp)
            {
                if (InPrimComp->IsRegistered() && InPrimComp->IsCollisionEnabled())
                {
                    Bounds += InPrimComp->Bounds.GetBox();
                    PrimitiveFound = true;
                }
            });
    }

    if (PrimitiveFound)
    {
        Bounds.GetCenterAndExtents(BoxOrigin, BoxExtent);
    }
    else
    {
        BoxOrigin = Actor.GetActorLocation();
        BoxExtent = FVector(50.f, 50.f, 50.f);
    }

    FVector2D ScreenPosMin, ScreenPosMax;
    if (ComputeBoundingBoxScreenPosition(PlayerController, BoxOrigin, BoxExtent, ScreenPosMin, ScreenPosMax))
    {
        const ImU32 Color = (&Actor == GetSelection()) ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 255, 255, 128);
        DrawList->AddRect(FCogImguiHelper::ToImVec2(ScreenPosMin) + Viewport->Pos, FCogImguiHelper::ToImVec2(ScreenPosMax) + Viewport->Pos, Color, 0.0f, 0, 1.0f);
        FCogWindowWidgets::AddTextWithShadow(DrawList, FCogImguiHelper::ToImVec2(ScreenPosMin + FVector2D(0, -14.0f)) + Viewport->Pos, Color, TCHAR_TO_ANSI(*GetActorName(Actor)));
    }
}

//-----------------------------------------------------------------------------------------
bool FCogEngineWindow_Selection::ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max)
{
    FVector Corners[8];
    Corners[0].Set(-Extent.X, -Extent.Y, -Extent.Z); // - - - 
    Corners[1].Set(Extent.X, -Extent.Y, -Extent.Z);  // + - - 
    Corners[2].Set(-Extent.X, Extent.Y, -Extent.Z);  // - + - 
    Corners[3].Set(-Extent.X, -Extent.Y, Extent.Z);  // - - + 
    Corners[4].Set(Extent.X, Extent.Y, -Extent.Z);   // + + - 
    Corners[5].Set(Extent.X, -Extent.Y, Extent.Z);   // + - + 
    Corners[6].Set(-Extent.X, Extent.Y, Extent.Z);   // - + + 
    Corners[7].Set(Extent.X, Extent.Y, Extent.Z);    // + + + 

    Min.X = FLT_MAX;
    Min.Y = FLT_MAX;
    Max.X = -FLT_MAX;
    Max.Y = -FLT_MAX;

    for (int i = 0; i < 8; ++i)
    {
        FVector2D ScreenLocation;
        if (PlayerController->ProjectWorldLocationToScreen(Origin + Corners[i], ScreenLocation, false) == false)
        {
            return false;
        }

        Min.X = FMath::Min(ScreenLocation.X, Min.X);
        Min.Y = FMath::Min(ScreenLocation.Y, Min.Y);
        Max.X = FMath::Max(ScreenLocation.X, Max.X);
        Max.Y = FMath::Max(ScreenLocation.Y, Max.Y);
    }

    // Prevent getting large values when the camera get close to the target
    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
    Min.X = FMath::Max(-DisplaySize.x, Min.X);
    Min.Y = FMath::Max(-DisplaySize.y, Min.Y);
    Max.X = FMath::Min(DisplaySize.x * 2, Max.X);
    Max.Y = FMath::Min(DisplaySize.y * 2, Max.Y);

    return true;
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
        if (ImGui::BeginPopup("SelectionPopup"))
        {
            ImGui::BeginChild("Popup", ImVec2(Width, FCogWindowWidgets::GetFontWidth() * 40), false);

            if (DrawSelectionCombo())
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndChild();
            ImGui::EndPopup();
        }

        AActor* GlobalSelection = FCogDebugSettings::GetSelection();

        //-----------------------------------
        // Selection
        //-----------------------------------
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
            FString CurrentSelectionName = GetActorName(GlobalSelection);

            if (ImGui::Button(TCHAR_TO_ANSI(*CurrentSelectionName), ImVec2(Width, 0.0f)))
            {
                ImGui::OpenPopup("SelectionPopup");
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Current Selection: %s", TCHAR_TO_ANSI(*CurrentSelectionName));
            }

            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(2);

            DrawActorContextMenu(GlobalSelection);
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
void FCogEngineWindow_Selection::SetGlobalSelection(AActor* Value) const
{
    FCogDebugSettings::SetSelection(GetWorld(), Value);
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