#include "CogAbilityWindow_Abilities.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogAbilityReplicator.h"
#include "CogImguiHelper.h"
#include "CogWindowWidgets.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
    bNoPadding = true;

    Asset = GetAsset<UCogAbilityDataAsset>();
    Config = GetConfig<UCogAbilityConfig_Abilities>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderHelp()
{
    ImGui::Text(
    "This window displays the gameplay abilities of the selected actor. "
    "Click the ability check box to force its activation or deactivation. "
    "Right click an ability to open or close the ability separate window. "
    "Use the 'Give Ability' menu to manually give an ability from a list defined in the '%s' data asset. "
    , TCHAR_TO_ANSI(*GetNameSafe(Asset.Get())));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderTick(float DetlaTime)
{
    Super::RenderTick(DetlaTime);

    RenderOpenAbilities();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderOpenAbilities()
{
    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Selection, true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    for (int i = OpenedAbilities.Num() - 1; i >= 0; --i)
    {
        FGameplayAbilitySpecHandle Handle = OpenedAbilities[i];

        FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
        if (Spec == nullptr)
        {
            continue;
        }

        UGameplayAbility* Ability = Spec->GetPrimaryInstance();
        if (Ability == nullptr)
        {
            Ability = Spec->Ability;
        }

        bool Open = true;
        if (ImGui::Begin(TCHAR_TO_ANSI(*GetAbilityName(Ability)), &Open))
        {
            RenderAbilityInfo(*AbilitySystemComponent, *Spec);
            ImGui::End();
        }

        if (Open == false)
        {
            OpenedAbilities.RemoveAt(i);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderContent()
{
    Super::RenderContent();

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        ImGui::TextDisabled("Invalid selection");
        return;
    }

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Selection, true);
    if (AbilitySystemComponent == nullptr)
    {
        ImGui::TextDisabled("Selection has no ability system component");
        return;
    }

    RenderAbiltiesMenu(Selection);

    RenderAbilitiesTable(*AbilitySystemComponent);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbiltiesMenu(AActor* Selection)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::BeginMenu("Give Ability"))
            {
                if (Asset != nullptr)
                {
                    int Index = 0;
                    for (TSubclassOf<UGameplayAbility> AbilityClass : Asset->Abilities)
                    {
                        ImGui::PushID(Index);

                        if (ImGui::MenuItem(TCHAR_TO_ANSI(*GetNameSafe(AbilityClass))))
                        {
                            if (ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetLocalReplicator(*GetWorld()))
                            {
                                Replicator->GiveAbility(Selection, AbilityClass);
                            }
                        }

                        ImGui::PopID();
                        Index++;
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();

            ImGui::Checkbox("Sort by Name", &Config->SortByName);
            ImGui::Checkbox("Show Active", &Config->ShowActive);
            ImGui::Checkbox("Show Inactive", &Config->ShowInactive);
            ImGui::Checkbox("Show Blocked", &Config->ShowBlocked);

            ImGui::Separator();

            ImGui::ColorEdit4("Active Color", (float*)&Config->ActiveColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Inactive Color", (float*)&Config->InactiveColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Blocked Color", (float*)&Config->BlockedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

            ImGui::Separator();

            if (ImGui::MenuItem("Reset"))
            {
                ResetConfig();
            }

            ImGui::EndMenu();
        }

        FCogWindowWidgets::MenuSearchBar(Filter);

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesTable(UAbilitySystemComponent& AbilitySystemComponent)
{
    TArray<FGameplayAbilitySpec>& Abilities = AbilitySystemComponent.GetActivatableAbilities();

    TArray<FGameplayAbilitySpec> FitleredAbilities;

    for (FGameplayAbilitySpec& Spec: Abilities)
    {
        const UGameplayAbility* Ability = Spec.GetPrimaryInstance();
        if (Ability == nullptr)
        {
            Ability = Spec.Ability;
        }

        const bool IsJustBlocked = Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent) == false;
        const bool IsJustActive = Spec.IsActive() && IsJustBlocked == false;
        const bool IsJustInactive = Spec.IsActive() == false && IsJustBlocked == false;

        if (Config->ShowBlocked == false && IsJustBlocked)
        {
            continue;
        }

        if (Config->ShowActive == false && IsJustActive)
        {
            continue;
        }

        if (Config->ShowInactive == false && IsJustInactive)
        {
            continue;
        }

        const auto AbilityName = StringCast<ANSICHAR>(*GetAbilityName(Ability));
        if (Filter.PassFilter(AbilityName.Get()) == false)
        {
            continue;
        }
        
        FitleredAbilities.Add(Spec);
    }

    if (Config->SortByName)
    {
        FitleredAbilities.Sort([](const FGameplayAbilitySpec& Lhs, const FGameplayAbilitySpec& Rhs)
        {
            return Lhs.Ability.GetName().Compare(Rhs.Ability.GetName()) < 0;
        });
    }
    
    if (ImGui::BeginTable("Abilities", 6, ImGuiTableFlags_SizingFixedFit
                                        | ImGuiTableFlags_Resizable
                                        | ImGuiTableFlags_NoBordersInBodyUntilResize
                                        | ImGuiTableFlags_ScrollY
                                        | ImGuiTableFlags_RowBg
                                        | ImGuiTableFlags_BordersV
                                        | ImGuiTableFlags_Reorderable
                                        | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("##Activation", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Ability");
        ImGui::TableSetupColumn("Level");
        ImGui::TableSetupColumn("Input");
        ImGui::TableSetupColumn("Cooldown");
        ImGui::TableSetupColumn("Blocked");
        ImGui::TableHeadersRow();

        static int SelectedIndex = -1;
        int Index = 0;

        for (FGameplayAbilitySpec& Spec : FitleredAbilities)
        {
            UGameplayAbility* Ability = Spec.GetPrimaryInstance();
            if (Ability == nullptr)
            {
                Ability = Spec.Ability;
            }

            ImGui::TableNextRow();

            ImGui::PushID(Index);

            const ImVec4 Color = GetAbilityColor(AbilitySystemComponent, Spec);
            ImGui::PushStyleColor(ImGuiCol_Text, Color);

            //------------------------
            // Activation
            //------------------------
            ImGui::TableNextColumn();
            FCogWindowWidgets::PushStyleCompact();
            bool IsActive = Spec.IsActive();
            if (ImGui::Checkbox("##Activation", &IsActive))
            {
                AbilityHandleToActivate = Spec.Handle;
            }
            FCogWindowWidgets::PopStyleCompact();

            //------------------------
            // Name
            //------------------------
            ImGui::TableNextColumn();

            if (ImGui::Selectable(TCHAR_TO_ANSI(*GetAbilityName(Ability)), SelectedIndex == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
            {
                SelectedIndex = Index;
                
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    OpenAbility(Spec.Handle);
                }
            }

            ImGui::PopStyleColor(1);

            //------------------------
            // Popup
            //------------------------
            if (ImGui::IsItemHovered())
            {
                FCogWindowWidgets::BeginTableTooltip();
                RenderAbilityInfo(AbilitySystemComponent, Spec);
                FCogWindowWidgets::EndTableTooltip();
            }

            //------------------------
            // ContextMenu
            //------------------------
            RenderAbilityContextMenu(AbilitySystemComponent, Spec, Index);

            //------------------------
            // Level
            //------------------------
            ImGui::TableNextColumn();
            ImGui::Text("%d", Spec.Level);

            //------------------------
            // InputPressed
            //------------------------
            ImGui::TableNextColumn();
            if (Spec.InputPressed > 0)
            {
                ImGui::Text("%d", Spec.InputPressed);
            }

            //------------------------
            // Cooldown
            //------------------------
            ImGui::TableNextColumn();
            RenderAbilityCooldown(AbilitySystemComponent, *Ability);

            //------------------------
            // Blocked
            //------------------------
            ImGui::TableNextColumn();
            const bool IsBlocked = Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent) == false;
            if (IsBlocked)
            {
                ImGui::Text("Blocked");
            }

            ImGui::PopID();
            Index++;
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityCooldown(const UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility& Ability)
{
    if (!Ability.IsInstantiated())
    {
        return;
    }

    FGameplayAbilitySpec* Spec = Ability.GetCurrentAbilitySpec();

    if (Spec == nullptr)
    {
        return;
    }

    float RemainingTime, CooldownDuration;
    Ability.GetCooldownTimeRemainingAndDuration(Ability.GetCurrentAbilitySpec()->Handle, AbilitySystemComponent.AbilityActorInfo.Get(), RemainingTime, CooldownDuration);

    if (CooldownDuration > 0)
    {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(100, 100, 100, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 100));
        ImGui::ProgressBar(RemainingTime / CooldownDuration, ImVec2(-1, ImGui::GetTextLineHeightWithSpacing() * 0.8f), TCHAR_TO_ANSI(*FString::Printf(TEXT("%.2f / %.2f"), RemainingTime, CooldownDuration)));
        ImGui::PopStyleColor(2);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityContextMenu(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, int Index)
{
    if (ImGui::BeginPopupContextItem())
    {
        bool bOpen = OpenedAbilities.Contains(Spec.Handle);
        if (ImGui::Checkbox("Open", &bOpen))
        {
            if (bOpen)
            {
                OpenAbility(Spec.Handle);
            }
            else
            {
                CloseAbility(Spec.Handle);
            }
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Remove"))
        {
            AbilityHandleToRemove = Spec.Handle;
        }

        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::OpenAbility(const FGameplayAbilitySpecHandle& Handle)
{
    OpenedAbilities.AddUnique(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::CloseAbility(const FGameplayAbilitySpecHandle& Handle)
{
    OpenedAbilities.Remove(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogAbilityWindow_Abilities::GetAbilityName(const UGameplayAbility* Ability)
{
    if (Ability == nullptr)
    {
        return FString("NULL");
    }

    FString Str = FCogAbilityHelper::CleanupName(Ability->GetName());
    return Str;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityInfo(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec)
{
    UGameplayAbility* Ability = Spec.GetPrimaryInstance();
    if (Ability == nullptr)
    {
        Ability = Spec.Ability;
    }

    if (Ability == nullptr)
    {
        return;
    }

    if (ImGui::BeginTable("Ability", 2, ImGuiTableFlags_Borders))
    {
        const ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");

        const ImVec4 Color = GetAbilityColor(AbilitySystemComponent, Spec);

        //------------------------
        // Name
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Name");
        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, Color);
        ImGui::Text("%s", TCHAR_TO_ANSI(*GetAbilityName(Ability)));
        ImGui::PopStyleColor(1);

        //------------------------
        // Activation
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Activation");
        ImGui::TableNextColumn();
        FCogWindowWidgets::PushStyleCompact();
        bool IsActive = Spec.IsActive();
        if (ImGui::Checkbox("##Activation", &IsActive))
        {
            AbilityHandleToActivate = Spec.Handle;
        }
        FCogWindowWidgets::PopStyleCompact();

        //------------------------
        // Active Count
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Active Count");
        ImGui::TableNextColumn();
        ImGui::Text("%u", Spec.ActiveCount);

        //------------------------
        // Cooldown
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Cooldown");
        ImGui::TableNextColumn();
        RenderAbilityCooldown(AbilitySystemComponent, *Ability);

        //------------------------
        // Handle
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Handle");
        ImGui::TableNextColumn();
        ImGui::Text("%s", TCHAR_TO_ANSI(*Spec.Handle.ToString()));

        //------------------------
        // Level
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Level");
        ImGui::TableNextColumn();
        ImGui::Text("%d", Spec.Level);

        //------------------------
        // InputID
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "InputID");
        ImGui::TableNextColumn();
        ImGui::Text("%d", Spec.InputID);

        //------------------------
        // InputPressed
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "InputPressed");
        ImGui::TableNextColumn();
        ImGui::Text("%d", Spec.InputPressed);

        //------------------------
        // AbilityTags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "AbilityTags");
        ImGui::TableNextColumn();
        const bool SatisfyTagRequirements = Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent);
        FCogAbilityHelper::RenderTagContainer(Ability->AbilityTags);

        //---------------------------------------------
        // TODO: find a way to display blocking tags
        //---------------------------------------------

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::GameTick(float DeltaTime)
{
    if (AbilityHandleToActivate.IsValid())
    {
        ProcessAbilityActivation(AbilityHandleToActivate);
        AbilityHandleToActivate = FGameplayAbilitySpecHandle();
    }

    if (AbilityHandleToRemove.IsValid())
    {
        if (AActor* Selection = GetSelection())
        {
            if (ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetLocalReplicator(*GetWorld()))
            {
                Replicator->RemoveAbility(Selection, AbilityHandleToRemove);
            }
        }

        AbilityHandleToRemove = FGameplayAbilitySpecHandle();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::ProcessAbilityActivation(const FGameplayAbilitySpecHandle& Handle)
{
    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Selection, true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
    if (Spec == nullptr)
    {
        return;
    }

    if (Spec->IsActive())
    {
        DeactivateAbility(*AbilitySystemComponent, *Spec); 
    }
    else
    {
        ActivateAbility(*AbilitySystemComponent, *Spec);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::ActivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec)
{
    Spec.InputPressed = true;
    AbilitySystemComponent.TryActivateAbility(Spec.Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::DeactivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec)
{
    Spec.InputPressed = false;
    AbilitySystemComponent.CancelAbilityHandle(Spec.Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogAbilityWindow_Abilities::GetAbilityColor(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec)
{
    UGameplayAbility* Ability = Spec.GetPrimaryInstance();
    if (Ability == nullptr)
    {
        Ability = Spec.Ability;
    }

    if (Spec.IsActive())
    {
        return FCogImguiHelper::ToImVec4(Config->ActiveColor);
    }

    if (Ability != nullptr && Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent) == false)
    {
        return FCogImguiHelper::ToImVec4(Config->BlockedColor);
    }

    return FCogImguiHelper::ToImVec4(Config->InactiveColor);
}