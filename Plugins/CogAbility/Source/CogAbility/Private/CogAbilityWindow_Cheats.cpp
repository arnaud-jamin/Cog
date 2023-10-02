#include "CogAbilityWindow_Cheats.h"

#include "CogAbilityDataAsset_Cheats.h"
#include "CogInterfacesAllegiance.h"
#include "CogDebugDraw.h"
#include "CogImguiHelper.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "imgui.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogAbilityWindow_Cheats::UCogAbilityWindow_Cheats()
{
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

    APawn* LocalPawn = GetLocalPlayerPawn();

    if (ImGui::BeginTable("Cheats", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("Toggle", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Instant", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        int Index = 0;
        for (const FCogAbilityCheat& CheatEffect : CheatsAsset->PersistentEffects)
        {
            ImGui::PushID(Index);
            AddCheat(LocalPawn, SelectedActor, CheatEffect, true);
            ImGui::PopID();
            Index++;
        }

        ImGui::TableNextColumn();

        for (const FCogAbilityCheat& CheatEffect : CheatsAsset->InstantEffects)
        {
            ImGui::PushID(Index);
            AddCheat(LocalPawn, SelectedActor, CheatEffect, false);
            ImGui::PopID();
            Index++;
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Cheats::AddCheat(AActor* CheatInstigator, AActor* SelectedActor, const FCogAbilityCheat& Cheat, bool IsPersistent)
{
    if (Cheat.Effect == nullptr)
    {
        return;
    }

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
            RequestCheat(CheatInstigator, SelectedActor, Cheat);
        }
    }
    else
    {
        if (ImGui::Button(TCHAR_TO_ANSI(*Cheat.Name), ImVec2(-1, 0)))
        {
            RequestCheat(CheatInstigator, SelectedActor, Cheat);
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
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Cheats::RequestCheat(AActor* CheatInstigator, AActor* SelectedActor, const FCogAbilityCheat& Cheat)
{
    const bool IsShiftDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Shift) != 0;
    const bool IsAltDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Alt) != 0;
    const bool IsControlDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Ctrl) != 0;

    TArray<AActor*> Actors;

    if (IsControlDown)
    {
        Actors.Add(CheatInstigator);
    }
    
    if (IsShiftDown || IsAltDown)
    {
        for (TActorIterator<ACharacter> It(GetWorld(), ACharacter::StaticClass()); It; ++It)
        {
            if (AActor* OtherActor = *It)
            {
                ECogInterfacesAllegiance Allegiance = ECogInterfacesAllegiance::Enemy;
                
                if (ICogInterfacesAllegianceActor* AllegianceInterface = Cast<ICogInterfacesAllegianceActor>(OtherActor))
                {
                    AllegianceInterface->GetAllegiance(CheatInstigator);
                }

                if ((IsShiftDown && (Allegiance == ECogInterfacesAllegiance::Enemy))
                    || (IsAltDown && (Allegiance == ECogInterfacesAllegiance::Ally)))
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

    FCogAbilityModule& Module = FCogAbilityModule::Get();
    if (ACogAbilityReplicator* Replicator = Module.GetLocalReplicator())
    {
        Replicator->ApplyCheat(CheatInstigator, Actors, Cheat);
    }
}
