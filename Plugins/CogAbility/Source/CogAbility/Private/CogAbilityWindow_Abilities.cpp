#include "CogAbilityWindow_Abilities.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogAbilityReplicator.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "CogDebugRob.h"
#include "imgui.h"

DEFINE_PRIVATE_ACCESSOR_VARIABLE(GameplayAbility_ActivationBlockedTags, UGameplayAbility, FGameplayTagContainer, ActivationBlockedTags);
DEFINE_PRIVATE_ACCESSOR_VARIABLE(GameplayAbility_ActivationRequiredTags, UGameplayAbility, FGameplayTagContainer, ActivationRequiredTags);

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

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
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    WindowFlags |= ImGuiWindowFlags_NoScrollbar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::PostBegin()
{
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

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

    RenderAbilitiesMenu(Selection);

    RenderAbilitiesTable(*AbilitySystemComponent);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesMenu(AActor* Selection)
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
                            if (ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetFirstReplicator(*GetWorld()))
                            {
                                Replicator->Server_GiveAbility(Selection, AbilityClass);
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

            RenderAbilitiesMenuColorSettings();

            ImGui::Separator();

            if (ImGui::MenuItem("Reset Settings"))
            {
                ResetConfig();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Filters"))
        {
            RenderAbilitiesMenuFilters();
            ImGui::EndMenu();
        }

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesMenuFilters()
{
    ImGui::Checkbox("Active", &Config->ShowActive);
    ImGui::Checkbox("Inactive", &Config->ShowInactive);
    ImGui::Checkbox("Pressed", &Config->ShowPressed);
    ImGui::Checkbox("Blocked", &Config->ShowBlocked);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesMenuColorSettings()
{
    ImGui::ColorEdit4("Active Color", &Config->ActiveAbilityColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::ColorEdit4("Inactive Color", &Config->InactiveAbilityColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::ColorEdit4("Blocked Color", &Config->BlockedAbilityColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::ColorEdit4("Default Tag Color", &Config->DefaultTagsColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::ColorEdit4("Blocked Tag Color", &Config->BlockedTagsColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::ColorEdit4("Input Pressed Color", &Config->InputPressedColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityActivation(FGameplayAbilitySpec& Spec)
{
    FCogWidgets::PushStyleCompact();
    bool IsActive = Spec.IsActive();
    if (ImGui::Checkbox("##Activation", &IsActive))
    {
        AbilityHandleToActivate = Spec.Handle;
    }
    FCogWidgets::PopStyleCompact();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesTableAbilityName(UAbilitySystemComponent& AbilitySystemComponent, int& SelectedIndex, int Index, FGameplayAbilitySpec& Spec, UGameplayAbility* Ability)
{
    const ImVec4 Color = GetAbilityColor(AbilitySystemComponent, Spec);
    ImGui::PushStyleColor(ImGuiCol_Text, Color);

    if (ImGui::Selectable(TCHAR_TO_ANSI(*GetAbilityName(Ability)), SelectedIndex == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
    {
        SelectedIndex = Index;
                
        if (ImGui::IsMouseDoubleClicked(0))
        {
            OpenAbilityDetails(Spec.Handle);
        }
    }

    ImGui::PopStyleColor(1);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesTableAbilityBlocking(UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility* Ability)
{
    if (Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent) == false)
    {
        FGameplayTagContainer OwnedGameplayTags;
        AbilitySystemComponent.GetOwnedGameplayTags(OwnedGameplayTags);

        if (const FGameplayTagContainer* ActivationBlockedTags = &PRIVATE_ACCESS_PTR(Ability, GameplayAbility_ActivationBlockedTags))
        {
            FGameplayTagContainer AllBlockingTags;
            AbilitySystemComponent.GetBlockedAbilityTags(AllBlockingTags);
            AllBlockingTags.AppendTags(OwnedGameplayTags);

            FCogAbilityHelper::RenderTagContainer(*ActivationBlockedTags, AllBlockingTags, false, true, true, ImVec4(0, 0, 0, 0), FCogImguiHelper::ToImVec4(Config->BlockedTagsColor));
        }

        ImGui::SameLine();
        if (const FGameplayTagContainer* ActivationRequiredTags = &PRIVATE_ACCESS_PTR(Ability, GameplayAbility_ActivationRequiredTags))
        {
            FCogAbilityHelper::RenderTagContainer(*ActivationRequiredTags, OwnedGameplayTags, true, true, true, ImVec4(0, 0, 0, 0), FCogImguiHelper::ToImVec4(Config->BlockedTagsColor));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesTable(UAbilitySystemComponent& AbilitySystemComponent)
{
    TArray<FGameplayAbilitySpec>& Abilities = AbilitySystemComponent.GetActivatableAbilities();

    TArray<FGameplayAbilitySpec> FilteredAbilities;

    for (FGameplayAbilitySpec& Spec : Abilities)
    {
        const UGameplayAbility* Ability = Spec.GetPrimaryInstance();
        if (Ability == nullptr)
        {
            Ability = Spec.Ability;
        }

        if (ShouldShowAbility(AbilitySystemComponent, Spec, Ability) == false)
        {
            continue;
        }

        const auto AbilityName = StringCast<ANSICHAR>(*GetAbilityName(Ability));
        if (Filter.PassFilter(AbilityName.Get()) == false)
        {
            continue;
        }
        
        FilteredAbilities.Add(Spec);
    }

    if (Config->SortByName)
    {
        FilteredAbilities.Sort([](const FGameplayAbilitySpec& Lhs, const FGameplayAbilitySpec& Rhs)
        {
            return Lhs.Ability.GetName().Compare(Rhs.Ability.GetName()) < 0;
        });
    }
    
    if (RenderAbilitiesTableHeader(AbilitySystemComponent))
    {
        static int SelectedIndex = -1;
        int Index = 0;

        for (FGameplayAbilitySpec& Spec : FilteredAbilities)
        {
            UGameplayAbility* Ability = Spec.GetPrimaryInstance();
            if (Ability == nullptr)
            {
                Ability = Spec.Ability;
            }

            ImGui::TableNextRow();

            ImGui::PushID(Index);

            RenderAbilitiesTableRow(AbilitySystemComponent, SelectedIndex, Index, Spec, Ability);

            ImGui::PopID();
            Index++;
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogAbilityWindow_Abilities::ShouldShowAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability)
{
    const bool IsBlocked = Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent) == false;
    if (Config->ShowBlocked && IsBlocked)
    {
        return true;
    }

    if (Config->ShowActive && Spec.IsActive())
    {
        return true;
    }

    if (Config->ShowInactive && Spec.IsActive() == false)
    {
        return true;
    }

    if (Config->ShowPressed && Spec.InputPressed)
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogAbilityWindow_Abilities::RenderAbilitiesTableHeader(UAbilitySystemComponent& AbilitySystemComponent)
{
    if (ImGui::BeginTable("Abilities", 6, ImGuiTableFlags_SizingFixedFit
        | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_NoBordersInBodyUntilResize
        | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_RowBg
        | ImGuiTableFlags_BordersV
        | ImGuiTableFlags_Reorderable
        | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("##ActivationCol", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Ability");
        ImGui::TableSetupColumn("Level");
        ImGui::TableSetupColumn("Input");
        ImGui::TableSetupColumn("Cooldown");
        ImGui::TableSetupColumn("Blocking");
        ImGui::TableHeadersRow();

        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilitiesTableRow(UAbilitySystemComponent& AbilitySystemComponent, int& SelectedIndex, int Index, FGameplayAbilitySpec& Spec, UGameplayAbility* Ability)
{
    //------------------------
    // Activation
    //------------------------
    ImGui::TableNextColumn();
    RenderAbilityActivation(Spec);

    //------------------------
    // Name
    //------------------------
    ImGui::TableNextColumn();
    RenderAbilitiesTableAbilityName(AbilitySystemComponent, SelectedIndex, Index, Spec, Ability);

    //------------------------
    // Popup
    //------------------------
    if (FCogWidgets::BeginItemTableTooltip())
    {
        RenderAbilityInfo(AbilitySystemComponent, Spec);
        FCogWidgets::EndItemTableTooltip();
    }

    //------------------------
    // ContextMenu
    //------------------------
    RenderAbilityContextMenu(AbilitySystemComponent, Spec, Index);

    //------------------------
    // Level
    //------------------------
    ImGui::TableNextColumn();
    RenderAbilityLevel(Spec);

    //------------------------
    // InputPressed
    //------------------------
    ImGui::TableNextColumn();
    RenderAbilityInputPressed(Spec);

    //------------------------
    // Cooldown
    //------------------------
    ImGui::TableNextColumn();
    RenderAbilityCooldown(AbilitySystemComponent, *Ability);

    //------------------------
    // Blocking
    //------------------------
    ImGui::TableNextColumn();
    RenderAbilitiesTableAbilityBlocking(AbilitySystemComponent, Ability);
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityCooldown(const UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility& Ability)
{
    if (!Ability.IsInstantiated())
    {
        return;
    }

    const FGameplayAbilitySpec* Spec = Ability.GetCurrentAbilitySpec();
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
void FCogAbilityWindow_Abilities::RenderAbilityLevel(FGameplayAbilitySpec& Spec)
{
    ImGui::Text("%d", Spec.Level);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityInputPressed(FGameplayAbilitySpec& Spec)
{
    if (Spec.InputPressed)
    {
        FCogWidgets::SmallButton("Pressed", FCogImguiHelper::ToImVec4(Config->ActiveAbilityColor));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityContextMenu(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, int Index)
{
    if (ImGui::BeginPopupContextItem())
    {
        bool bOpen = OpenedAbilities.Contains(Spec.Handle);
        if (ImGui::Checkbox("Open Details", &bOpen))
        {
            if (bOpen)
            {
                OpenAbilityDetails(Spec.Handle);
            }
            else
            {
                CloseAbilityDetails(Spec.Handle);
            }
            ImGui::CloseCurrentPopup();
        }

        const ImVec2 ButtonsSize = ImVec2(ImGui::GetFontSize() * 10, 0);

        if (ImGui::Button("Cancel", ButtonsSize))
        {
            AbilitySystemComponent.CancelAbilityHandle(Spec.Handle);
        }

        if (ImGui::Button("Remove", ButtonsSize))
        {
            AbilityHandleToRemove = Spec.Handle;
        }

        FCogWidgets::OpenObjectAssetButton(Spec.Ability, ButtonsSize);

        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::OpenAbilityDetails(const FGameplayAbilitySpecHandle& Handle)
{
    OpenedAbilities.AddUnique(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::CloseAbilityDetails(const FGameplayAbilitySpecHandle& Handle)
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

    if (ImGui::BeginTable("Ability", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {
        constexpr ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

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
        FCogWidgets::PushStyleCompact();
        bool IsActive = Spec.IsActive();
        if (ImGui::Checkbox("##Activation", &IsActive))
        {
            AbilityHandleToActivate = Spec.Handle;
        }
        FCogWidgets::PopStyleCompact();

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
        RenderAbilityLevel(Spec);

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
        ImGui::TextColored(TextColor, "Input Pressed");
        ImGui::TableNextColumn();
        RenderAbilityInputPressed(Spec);

        //------------------------
        // AbilityTags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Ability Tags");
        ImGui::TableNextColumn();
        FCogAbilityHelper::RenderTagContainer(Ability->GetAssetTags());

        //------------------------
        // RequiredTags
        //------------------------
        FGameplayTagContainer OwnedGameplayTags;
        AbilitySystemComponent.GetOwnedGameplayTags(OwnedGameplayTags);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Required Tags");
        ImGui::TableNextColumn();
        if (const FGameplayTagContainer* ActivationRequiredTags = &PRIVATE_ACCESS_PTR(Ability, GameplayAbility_ActivationRequiredTags))
        {
            FCogAbilityHelper::RenderTagContainer(*ActivationRequiredTags, OwnedGameplayTags, true, false, false, FCogImguiHelper::ToImVec4(Config->DefaultTagsColor), FCogImguiHelper::ToImVec4(Config->BlockedTagsColor));
        }

        //------------------------
        // BlockingTags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Blocking Tags");
        ImGui::TableNextColumn();
        if (const FGameplayTagContainer* ActivationBlockedTags = &PRIVATE_ACCESS_PTR(Ability, GameplayAbility_ActivationBlockedTags))
        {
            FGameplayTagContainer AllBlockingTags;
            AbilitySystemComponent.GetBlockedAbilityTags(AllBlockingTags);
            AllBlockingTags.AppendTags(OwnedGameplayTags);

            FCogAbilityHelper::RenderTagContainer(*ActivationBlockedTags, AllBlockingTags, false, false, false, FCogImguiHelper::ToImVec4(Config->DefaultTagsColor), FCogImguiHelper::ToImVec4(Config->BlockedTagsColor));
        }

        //------------------------
        // Additional info
        //------------------------
        RenderAbilityAdditionalInfo(AbilitySystemComponent, Spec, *Ability, TextColor);

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Abilities::RenderAbilityAdditionalInfo(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, UGameplayAbility& Ability, const ImVec4& TextColor)
{

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
            if (ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetFirstReplicator(*GetWorld()))
            {
                Replicator->Server_RemoveAbility(Selection, AbilityHandleToRemove);
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
        return FCogImguiHelper::ToImVec4(Config->ActiveAbilityColor);
    }

    if (Ability != nullptr && Ability->DoesAbilitySatisfyTagRequirements(AbilitySystemComponent) == false)
    {
        return FCogImguiHelper::ToImVec4(Config->BlockedAbilityColor);
    }

    return FCogImguiHelper::ToImVec4(Config->InactiveAbilityColor);
}