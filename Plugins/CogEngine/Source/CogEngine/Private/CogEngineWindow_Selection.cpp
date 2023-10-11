#include "CogEngineWindow_Selection.h"

#include "CogDebugDraw.h"
#include "CogImguiModule.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Selection::RenderHelp()
{
    ImGui::Text(
        "This window can be used to select an actor either by picking an actor in the world, "
        "or by selecting an actor in the actor list. "
        "The actor list can be filtered by actor type (Actor, Character, etc). "
        "The current selection is used by various debug windows to filter out their content"
        );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Selection::UCogEngineWindow_Selection()
{
    bHasMenu = true;
    SubClasses = { AActor::StaticClass(), ACharacter::StaticClass() };
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Selection::ToggleSelectionMode()
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
void UCogEngineWindow_Selection::ActivateSelectionMode()
{
    bSelectionModeActive = true;
    bImGuiHadInputBeforeEnteringSelectionMode = FCogImguiModule::Get().GetEnableInput();
    FCogImguiModule::Get().SetEnableInput(true);
    GetOwner()->SetHideAllWindows(true);
}

//--------------------------------------------------------------------------------------------------------------------------

void UCogEngineWindow_Selection::HackWaitInputRelease()
{
    WaitInputReleased = 1;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Selection::DeactivateSelectionMode()
{
    bSelectionModeActive = false;

    //--------------------------------------------------------------------------------------------
    // We can enter selection mode by a command, and imgui might not have the input focus
    // When in selection mode we need imgui to have the input focus
    // When leaving selection mode we want to leave it as is was before
    //--------------------------------------------------------------------------------------------
    if (bImGuiHadInputBeforeEnteringSelectionMode == false)
    {
        FCogImguiModule::Get().SetEnableInput(false);
    }

    GetOwner()->SetHideAllWindows(false);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Selection::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (FCogDebugSettings::GetSelection() == nullptr)
    {
        FCogDebugSettings::SetSelection(GetLocalPlayerPawn());
    }

    if (bSelectionModeActive)
    {
        TickSelectionMode();
    }

    if (AActor* Actor = GetSelection())
    {
        if (Actor != GetLocalPlayerPawn())
        {
            DrawActorFrame(Actor);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Selection::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Pick"))
        {
            ActivateSelectionMode();
            HackWaitInputRelease();
        }

        ImGui::EndMenuBar();
    }

    DrawSelectionCombo();
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Selection::DrawSelectionCombo()
{
    bool SelectionChanged = false;

    APawn* LocalPlayerPawn = GetLocalPlayerPawn();

    //------------------------
    // Actor Class Combo
    //------------------------

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##SelectionType", TCHAR_TO_ANSI(*GetNameSafe(SelectedSubClass))))
    {
        for (TSubclassOf<AActor> ItSubClass : SubClasses)
        {
            if (ImGui::Selectable(TCHAR_TO_ANSI(*GetNameSafe(ItSubClass)), false))
            {
                SelectedSubClass = ItSubClass;
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
    for (TActorIterator<AActor> It(GetWorld(), SelectedSubClass); It; ++It)
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
            const FString ActorName = GetNameSafe(Actor);
            if (ImGui::Selectable(TCHAR_TO_ANSI(*ActorName), bIsSelected))
            {
                FCogDebugSettings::SetSelection(Actor);
                SelectionChanged = true;
            }

            ImGui::PopStyleColor(1);

            DrawActorContextMenu(Actor);

            //------------------------
            // Draw Frame
            //------------------------
            if (ImGui::IsItemHovered())
            {
                DrawActorFrame(Actor);
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

void UCogEngineWindow_Selection::DrawActorContextMenu(AActor* Actor)
{
    //------------------------
    // ContextMenu
    //------------------------
    ImGui::SetNextWindowSize(ImVec2(FCogWindowWidgets::GetFontWidth() * 20, 0));
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Button("Reset Selection", ImVec2(-1, 0)))
        {
            ImGui::CloseCurrentPopup();
            FCogDebugSettings::SetSelection(GetLocalPlayerPawn());
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

        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Selection::TickSelectionMode()
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        DeactivateSelectionMode();
        return;
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AnyWindow))
    {
        return;
    }

    APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        DeactivateSelectionMode();
        return;
    }

    ImDrawList* DrawList = ImGui::GetBackgroundDrawList();
    DrawList->AddRect(ImVec2(0, 0), ImGui::GetIO().DisplaySize, IM_COL32(255, 0, 0, 128), 0.0f, 0, 20.0f);
    FCogWindowWidgets::AddTextWithShadow(DrawList, ImVec2(20, 20), IM_COL32(255, 255, 255, 255), "Picking Mode. \n[LMB] Pick \n[RMB] Cancel");

    AActor* HoveredActor = nullptr;

    FVector WorldOrigin, WorldDirection;
    if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, FCogImguiHelper::ToVector2D(ImGui::GetMousePos()), WorldOrigin, WorldDirection))
    {
        TArray<AActor*> IgnoreList;

        FHitResult HitResult;
        if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), WorldOrigin, WorldOrigin + WorldDirection * 10000, TraceType, false, IgnoreList, EDrawDebugTrace::None, HitResult, true))
        {
            if (HitResult.GetActor() != nullptr)
            {
                if (HitResult.GetActor()->GetClass()->IsChildOf(SelectedSubClass))
                {
                    HoveredActor = HitResult.GetActor();
                }
            }
        }
    }

    if (HoveredActor != nullptr)
    {
        DrawActorFrame(HoveredActor);
    }

    if (bSelectionModeActive)
    {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (WaitInputReleased == 0)
            {
                if (HoveredActor != nullptr)
                {
                    FCogDebugSettings::SetSelection(HoveredActor);
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
void UCogEngineWindow_Selection::DrawActorFrame(const AActor* Actor)
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

    FVector BoxOrigin, BoxExtent;

    bool PrimitiveFound = false;
    FBox Bounds(ForceInit);
    
    if (const UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    {
        PrimitiveFound = true;
        Bounds += PrimitiveComponent->Bounds.GetBox();
    }
    else
    {
        Actor->ForEachComponent<UPrimitiveComponent>(true, [&](const UPrimitiveComponent* InPrimComp)
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
        BoxOrigin = Actor->GetActorLocation();
        BoxExtent = FVector(50.f, 50.f, 50.f);
    }

    FVector2D ScreenPosMin, ScreenPosMax;
    if (ComputeBoundingBoxScreenPosition(PlayerController, BoxOrigin, BoxExtent, ScreenPosMin, ScreenPosMax))
    {
        const ImU32 Color = (Actor == GetSelection()) ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 255, 255, 128);
        DrawList->AddRect(FCogImguiHelper::ToImVec2(ScreenPosMin), FCogImguiHelper::ToImVec2(ScreenPosMax), Color, 0.0f, 0, 1.0f);
        FCogWindowWidgets::AddTextWithShadow(DrawList, FCogImguiHelper::ToImVec2(ScreenPosMin + FVector2D(0, -14.0f)), Color, TCHAR_TO_ANSI(*Actor->GetName()));
    }
}

//-----------------------------------------------------------------------------------------
bool UCogEngineWindow_Selection::ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max)
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
void UCogEngineWindow_Selection::DrawMainMenuWidget(bool Draw, float& Width)
{
    const float PickButtonWidth = FCogWindowWidgets::GetFontWidth() * 6;
    const float SelectionButtonWidth = FCogWindowWidgets::GetFontWidth() * 30;
    const float ResetButtonWidth = FCogWindowWidgets::GetFontWidth() * 2;
    Width = PickButtonWidth + SelectionButtonWidth + ResetButtonWidth;

    if (Draw == false)
    {
        return;
    }

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

    //-----------------------------------
    // Pick Button
    //-----------------------------------
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

        if (ImGui::Button("Pick", ImVec2(PickButtonWidth, 0)))
        {
            ActivateSelectionMode();
            HackWaitInputRelease();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Enter picking mode to select an actor on screen.");
        }

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(2);
    }

    AActor* GlobalSelection = FCogDebugSettings::GetSelection();

    //-----------------------------------
    // Selection
    //-----------------------------------
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4((ImGuiCol_FrameBg)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4((ImGuiCol_FrameBgActive)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4((ImGuiCol_FrameBgHovered)));
        ImGui::SameLine();
        FString SelectionName = GetNameSafe(GlobalSelection);

        if (ImGui::Button(TCHAR_TO_ANSI(*SelectionName), ImVec2(SelectionButtonWidth, 0)))
        {
            ImGui::OpenPopup("SelectionPopup");
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Current Selection: %s", TCHAR_TO_ANSI(*SelectionName));
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        DrawActorContextMenu(GlobalSelection);
    }

    //-----------------------------------
    // Reset Button
    //-----------------------------------
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4((ImGuiCol_FrameBg)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4((ImGuiCol_FrameBgActive)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4((ImGuiCol_FrameBgHovered)));
        ImGui::SameLine();
        if (ImGui::Button("X", ImVec2(ResetButtonWidth, 0)))
        {
            FCogDebugSettings::SetSelection(nullptr);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Reset the selection to the controlled actor.");
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(1);
    }
}