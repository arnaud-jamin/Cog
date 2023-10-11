#include "CogAbilityWindow_Cheats.h"

#include "AbilitySystemGlobals.h"
#include "CogAbilityDataAsset_Cheats.h"
#include "CogAbilityReplicator.h"
#include "CogCommonAllegianceActorInterface.h"
#include "CogDebugDraw.h"
#include "CogImguiHelper.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "imgui.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Cheats::RenderHelp()
{
    ImGui::Text(
        "This window can be used to apply cheats to the selected actor (by default). "
        "The cheats can be configured in the '%s' data asset. "
        "When clicking a cheat button, press:\n"
        "   [CTRL]  to apply the cheat to controlled actor\n"
        "   [ALT]   to apply the cheat to the allies of the selected actor\n"
        "   [SHIFT] to apply the cheat to the enemies of the selected actor\n"
        , TCHAR_TO_ANSI(*GetNameSafe(CheatsAsset.Get()))
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogAbilityWindow_Cheats::UCogAbilityWindow_Cheats()
{
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Cheats::SetCheatsAsset(const UCogAbilityDataAsset_Cheats* Value) 
{
    CheatsAsset = Value;

    if (bReapplyCheatsBetweenPlays == false)
    {
        return;
    }

    static int32 IsFirstLaunch = true;
    if (IsFirstLaunch && bReapplyCheatsBetweenLaunches == false)
    {
        return;
    }
    IsFirstLaunch = false;

    if (CheatsAsset == nullptr)
    {
        return;
    }

    APawn* LocalPawn = GetLocalPlayerPawn();
    if (LocalPawn == nullptr)
    {
        return;
    }

    ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        return;
    }

    TArray<AActor*> Targets{ LocalPawn };

    for (const FString& AppliedCheatName : AppliedCheats)
    {
        if (const FCogAbilityCheat* Cheat = CheatsAsset->PersistentEffects.FindByPredicate(
            [AppliedCheatName](const FCogAbilityCheat& Cheat) { return Cheat.Name == AppliedCheatName; }))
        {
            Replicator->ApplyCheat(LocalPawn, Targets, *Cheat);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Cheats::RenderContent()
{
    Super::RenderContent();

    AActor* SelectedActor = GetSelection();
    if (SelectedActor == nullptr)
    {
        return;
    }

    if (CheatsAsset == nullptr)
    {
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Reapply Cheats Between Plays", &bReapplyCheatsBetweenPlays);

            if (bReapplyCheatsBetweenPlays == false)
            {
                ImGui::BeginDisabled();
            }
            ImGui::Checkbox("Reapply Cheats Between Launches", &bReapplyCheatsBetweenLaunches);

            if (bReapplyCheatsBetweenPlays == false)
            {
                ImGui::EndDisabled();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    APawn* ControlledActor = GetLocalPlayerPawn();

    if (ImGui::BeginTable("Cheats", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("Toggle", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Instant", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        bool bHasChanged = false;

        int Index = 0;
        for (const FCogAbilityCheat& CheatEffect : CheatsAsset->PersistentEffects)
        {
            ImGui::PushID(Index);
            bHasChanged |= AddCheat(ControlledActor, SelectedActor, CheatEffect, true);
            ImGui::PopID();
            Index++;
        }

        //----------------------------------------------------------------
        // When a persistent cheat has been changed, update the applied 
        // cheats string array to be saved in the config later
        //----------------------------------------------------------------
        if (bHasChanged && SelectedActor == ControlledActor)
        {
            AppliedCheats.Empty();

            for (const FCogAbilityCheat& CheatEffect : CheatsAsset->PersistentEffects)
            {
                if (ACogAbilityReplicator::IsCheatActive(SelectedActor, CheatEffect))
                {
                    AppliedCheats.Add(CheatEffect.Name);
                }
            }
        }

        ImGui::TableNextColumn();

        for (const FCogAbilityCheat& CheatEffect : CheatsAsset->InstantEffects)
        {
            ImGui::PushID(Index);
            AddCheat(ControlledActor, SelectedActor, CheatEffect, false);
            ImGui::PopID();
            Index++;
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogAbilityWindow_Cheats::AddCheat(AActor* ControlledActor, AActor* SelectedActor, const FCogAbilityCheat& Cheat, bool IsPersistent)
{
    if (Cheat.Effect == nullptr)
    {
        return false;
    }

    bool bIsPressed = false;

    const FGameplayTagContainer& Tags = Cheat.Effect->GetDefaultObject<UGameplayEffect>()->InheritableGameplayEffectTags.CombinedTags;

    FLinearColor Color;
    if (Tags.HasTag(CheatsAsset->NegativeEffectTag))
    {
        Color = CheatsAsset->NegativeEffectColor;
    }
    else if (Tags.HasTag(CheatsAsset->PositiveEffectTag))
    {
        Color = CheatsAsset->PositiveEffectColor;
    }
    else
    {
        Color = CheatsAsset->NeutralEffectColor;
    }
        
    FCogWindowWidgets::PushBackColor(FCogImguiHelper::ToImVec4(Color));

    if (IsPersistent)
    {
        bool isEnabled = ACogAbilityReplicator::IsCheatActive(SelectedActor, Cheat);
        if (ImGui::Checkbox(TCHAR_TO_ANSI(*Cheat.Name), &isEnabled))
        {
            RequestCheat(ControlledActor, SelectedActor, Cheat);
            bIsPressed = true;
        }
    }
    else
    {
        if (ImGui::Button(TCHAR_TO_ANSI(*Cheat.Name), ImVec2(-1, 0)))
        {
            RequestCheat(ControlledActor, SelectedActor, Cheat);
            bIsPressed = true;
        }
    }

    if (ImGui::IsItemHovered())
    {
        const bool IsShiftDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Shift) != 0;
        const bool IsAltDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Alt) != 0;
        const bool IsControlDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Ctrl) != 0;

        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsShiftDown || IsAltDown || IsControlDown ? 0.5f : 1.0f),       "On Selection");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsShiftDown ? 1.0f : 0.5f),                                     "On Enemies    [SHIFT]");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsAltDown ? 1.0f : 0.5f),                                       "On Allies     [ALT]");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f),                                   "On Controlled [CTRL]");
        ImGui::EndTooltip();
    }

    FCogWindowWidgets::PopBackColor();

    return bIsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Cheats::RequestCheat(AActor* ControlledActor, AActor* SelectedActor, const FCogAbilityCheat& Cheat)
{
    const bool IsShiftDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Shift) != 0;
    const bool IsAltDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Alt) != 0;
    const bool IsControlDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Ctrl) != 0;

    TArray<AActor*> Actors;

    if (IsControlDown)
    {
        Actors.Add(ControlledActor);
    }
    
    if (IsShiftDown || IsAltDown)
    {
        for (TActorIterator<ACharacter> It(GetWorld(), ACharacter::StaticClass()); It; ++It)
        {
            if (AActor* OtherActor = *It)
            {
                ECogCommonAllegiance Allegiance = ECogCommonAllegiance::Enemy;
                
                if (ICogCommonAllegianceActorInterface* AllegianceInterface = Cast<ICogCommonAllegianceActorInterface>(OtherActor))
                {
                    AllegianceInterface->GetAllegianceWithOtherActor(ControlledActor);
                }

                if ((IsShiftDown && (Allegiance == ECogCommonAllegiance::Enemy))
                    || (IsAltDown && (Allegiance == ECogCommonAllegiance::Friendly)))
                {
                    Actors.Add(OtherActor);
                }
            }
        }
    }

    if ((IsControlDown || IsShiftDown || IsAltDown) == false)
    {
        Actors.Add(SelectedActor);
    }

    if (ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetLocalReplicator(*GetWorld()))
    {
        Replicator->ApplyCheat(ControlledActor, Actors, Cheat);
    }
}
