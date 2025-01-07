#include "CogAbilityWindow_Cheats.h"

#include "CogAbilityConfig_Alignment.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityReplicator.h"
#include "CogCommonAllegianceActorInterface.h"
#include "CogImguiHelper.h"
#include "CogWindowConsoleCommandManager.h"
#include "CogWindowWidgets.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "imgui.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Asset = GetAsset<UCogAbilityDataAsset>();
    Config = GetConfig<UCogAbilityConfig_Cheats>();
    AlignmentConfig = GetConfig<UCogAbilityConfig_Alignment>();

    FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
        TEXT("Cog.Cheat"),
        TEXT("Apply a cheat to the selection. Cog.Cheat <CheatName> -Allies -Enemies -Controlled"),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
            {
                if (InArgs.Num() > 0)
                {
                    if (const FCogAbilityCheat* cheat = FindCheatByName(InArgs[0]))
                    {
                        const bool ApplyToEnemies = InArgs.Contains("-Enemies");
                        const bool ApplyToAllies = InArgs.Contains("-Allies");
                        const bool ApplyToControlled = InArgs.Contains("-Controlled");

                        RequestCheat(GetLocalPlayerPawn(), GetSelection(), *cheat, ApplyToEnemies, ApplyToAllies, ApplyToControlled);
                    }
                    else
                    {
                        UE_LOG(LogCogImGui, Warning, TEXT("Cog.Cheat %s | Cheat not found"), *InArgs[0]);
                    }
                }
            }));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::RenderHelp()
{
    ImGui::Text(
        "This window can be used to apply cheats to the selected actor (by default). "
        "The cheats can be configured in the '%s' data asset. "
        "When clicking a cheat button, press:\n"
        "   [CTRL]  to apply the cheat to controlled actor\n"
        "   [ALT]   to apply the cheat to the allies of the selected actor\n"
        "   [SHIFT] to apply the cheat to the enemies of the selected actor\n"
        , TCHAR_TO_ANSI(*GetNameSafe(Asset.Get()))
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
    AlignmentConfig->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::GameTick(float DeltaTime)
{
    Super::GameTick(DeltaTime);

    TryReapplyCheats();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::TryReapplyCheats()
{
    if (Config == nullptr)
    {
        return;
    }
    
    if (bHasReappliedCheats)
    {
        return;
    }

    if (Config->bReapplyCheatsBetweenPlays == false)
    {
        return;
    }

    static int32 IsFirstLaunch = true;
    if (IsFirstLaunch && Config->bReapplyCheatsBetweenLaunches == false)
    {
        return;
    }
    IsFirstLaunch = false;

    if (Asset == nullptr)
    {
        return;
    }

    APawn* ControlledActor = GetLocalPlayerPawn();
    if (ControlledActor == nullptr)
    {
        return;
    }

    ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetFirstReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        return;
    }

    TArray<AActor*> Targets { ControlledActor };

    for (int32 i = Config->AppliedCheats.Num() - 1; i >= 0; i--)
    {
        const FString& AppliedCheatName = Config->AppliedCheats[i];

        if (const FCogAbilityCheat* Cheat = Asset->PersistentEffects.FindByPredicate(
            [AppliedCheatName](const FCogAbilityCheat& Cheat) { return Cheat.Name == AppliedCheatName; }))
        {
            Replicator->Server_ApplyCheat(ControlledActor, Targets, *Cheat);
        }
        else
        {
            //-----------------------------------------------------
            // This cheat doesn't exist anymore. We can remove it.
            //-----------------------------------------------------
            Config->AppliedCheats.RemoveAt(i);
        }
    }

    bHasReappliedCheats = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::RenderContent()
{
    Super::RenderContent();

    if (Config == nullptr)
    {
        ImGui::TextDisabled("Invalid Config");
        return;
    }

    AActor* SelectedActor = GetSelection();
    if (SelectedActor == nullptr)
    {
        ImGui::TextDisabled("Invalid Selection");
        return;
    }

    if (Asset == nullptr)
    {
        ImGui::TextDisabled("Invalid Asset");
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Reapply Cheats Between Plays", &Config->bReapplyCheatsBetweenPlays);

            if (Config->bReapplyCheatsBetweenPlays == false)
            {
                ImGui::BeginDisabled();
            }
            ImGui::Checkbox("Reapply Cheats Between Launches", &Config->bReapplyCheatsBetweenLaunches);

            if (Config->bReapplyCheatsBetweenPlays == false)
            {
                ImGui::EndDisabled();
            }

            ImGui::Separator();
            ImGui::ColorEdit4("Positive Color", (float*)&AlignmentConfig->PositiveColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Negative Color", (float*)&AlignmentConfig->NegativeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Neutral Color", (float*)&AlignmentConfig->NeutralColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            
            ImGui::Separator();
            if (ImGui::MenuItem("Reset"))
            {
                ResetConfig();
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


        int Index = 0;
        for (const FCogAbilityCheat& CheatEffect : Asset->PersistentEffects)
        {
            ImGui::PushID(Index);
            AddCheat(ControlledActor, SelectedActor, CheatEffect, true);
            ImGui::PopID();
            Index++;
        }

        //----------------------------------------------------------------------------
        // Update the config of applied cheat to reapply them on the next launch. 
        // We do not updated them only when the the user input is pressed because
        // the state of the cheat is lagging when connected to a server. 
        // So we check if the array should be updated all the time.
        //----------------------------------------------------------------------------
        if (SelectedActor == ControlledActor)
        {
            for (const FCogAbilityCheat& CheatEffect : Asset->PersistentEffects)
            {
                if (ACogAbilityReplicator::IsCheatActive(SelectedActor, CheatEffect))
                {
                    Config->AppliedCheats.AddUnique(CheatEffect.Name);
                }
                else
                {
                    Config->AppliedCheats.Remove(CheatEffect.Name);
                }
            }
        }

        ImGui::TableNextColumn();

        for (const FCogAbilityCheat& CheatEffect : Asset->InstantEffects)
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
bool FCogAbilityWindow_Cheats::AddCheat(AActor* ControlledActor, AActor* SelectedActor, const FCogAbilityCheat& Cheat, bool IsPersistent)
{
    if (Cheat.Effect == nullptr)
    {
        return false;
    }

    const UGameplayEffect* EffectCDO = Cheat.Effect->GetDefaultObject<UGameplayEffect>();
    
    if (EffectCDO != nullptr)
    {
        FCogWindowWidgets::PushBackColor(FCogImguiHelper::ToImVec4(AlignmentConfig->GetEffectColor(Asset, *EffectCDO)));
    }

    const bool IsShiftDown      = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Shift) != 0;
    const bool IsAltDown        = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Alt) != 0;
    const bool IsControlDown    = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Ctrl) != 0;

    bool bIsPressed = false;
    if (IsPersistent)
    {
        bool isEnabled = ACogAbilityReplicator::IsCheatActive(SelectedActor, Cheat);
        if (ImGui::Checkbox(TCHAR_TO_ANSI(*Cheat.Name), &isEnabled))
        {
            RequestCheat(ControlledActor, SelectedActor, Cheat, IsShiftDown, IsAltDown, IsControlDown);
            bIsPressed = true;
        }
    }
    else
    {
        if (ImGui::Button(TCHAR_TO_ANSI(*Cheat.Name), ImVec2(-1, 0)))
        {
            RequestCheat(ControlledActor, SelectedActor, Cheat, IsShiftDown, IsAltDown, IsControlDown);
            bIsPressed = true;
        }
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsShiftDown || IsAltDown || IsControlDown ? 0.5f : 1.0f),   "On Selection");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsShiftDown ? 1.0f : 0.5f),                                 "On Enemies    [SHIFT]");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsAltDown ? 1.0f : 0.5f),                                   "On Allies     [ALT]");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f),                               "On Controlled [CTRL]");
        ImGui::EndTooltip();
    }

    if (EffectCDO != nullptr)
    {
        FCogWindowWidgets::PopBackColor();
    }

    return bIsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Cheats::RequestCheat(AActor* ControlledActor, AActor* SelectedActor, const FCogAbilityCheat& Cheat, bool ApplyToEnemies, bool ApplyToAllies, bool ApplyToControlled)
{
    TArray<AActor*> Actors;

    if (ApplyToControlled)
    {
        Actors.Add(ControlledActor);
    }
    
    if (ApplyToEnemies || ApplyToAllies)
    {
        for (TActorIterator<ACharacter> It(GetWorld(), ACharacter::StaticClass()); It; ++It)
        {
            if (AActor* OtherActor = *It)
            {
                ECogCommonAllegiance Allegiance = ECogCommonAllegiance::Enemy;
                
                if (ICogCommonAllegianceActorInterface* AllegianceInterface = Cast<ICogCommonAllegianceActorInterface>(OtherActor))
                {
                    Allegiance = AllegianceInterface->GetAllegianceWithOtherActor(ControlledActor);
                }

                if ((ApplyToEnemies && (Allegiance == ECogCommonAllegiance::Enemy))
                    || (ApplyToAllies && (Allegiance == ECogCommonAllegiance::Friendly)))
                {
                    Actors.Add(OtherActor);
                }
            }
        }
    }

    if ((ApplyToControlled || ApplyToEnemies || ApplyToAllies) == false)
    {
        Actors.Add(SelectedActor);
    }

    if (ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetFirstReplicator(*GetWorld()))
    {
        Replicator->Server_ApplyCheat(ControlledActor, Actors, Cheat);
    }
    else
    {
        UE_LOG(LogCogImGui, Warning, TEXT("FCogAbilityWindow_Cheats::RequestCheat | Replicator not found"));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
const FCogAbilityCheat* FCogAbilityWindow_Cheats::FindCheatByName(const FString& CheatName)
{
    for (const FCogAbilityCheat& cheat : Asset->PersistentEffects)
    {
        if (cheat.Name == CheatName)
        {
            return &cheat;
        }
    }

    for (const FCogAbilityCheat& cheat : Asset->InstantEffects)
    {
        if (cheat.Name == CheatName)
        {
            return &cheat;
        }
    }

    return nullptr;
}
