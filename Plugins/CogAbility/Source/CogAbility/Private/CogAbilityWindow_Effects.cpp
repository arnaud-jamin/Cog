#include "CogAbilityWindow_Effects.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AttributeSet.h"
#include "CogAbilityConfig_Alignment.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Asset = GetAsset<UCogAbilityDataAsset>();
    Config = GetConfig<UCogAbilityConfig_Effects>();
    AlignmentConfig = GetConfig<UCogAbilityConfig_Alignment>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderHelp()
{
    ImGui::Text(
        "This window displays the gameplay effects of the selected actor. "
        "Mouse over an effect to see its details such as its modifiers and the gameplay tags it grants"
        );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    RenderOpenEffects();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::PostBegin()
{
    ImGui::PopStyleVar();
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Sort by Name", &Config->SortByName);
            ImGui::Checkbox("Sort by Alignment", &Config->SortByAlignment);
            
            ImGui::Separator();
            ImGui::ColorEdit4("Positive Color", &AlignmentConfig->PositiveColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Negative Color", &AlignmentConfig->NegativeColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Neutral Color", &AlignmentConfig->NeutralColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            
            ImGui::Separator();
            if (ImGui::MenuItem("Reset"))
            {
                ResetConfig();
            }
            ImGui::EndMenu();
        }

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }

    RenderEffectsTable();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderEffectsTable()
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        ImGui::TextDisabled("Selection has no ability system component");
        return;
    }

    if (ImGui::BeginTable("Effects", 5, ImGuiTableFlags_SizingFixedFit
                                        | ImGuiTableFlags_Resizable
                                        | ImGuiTableFlags_NoBordersInBodyUntilResize
                                        | ImGuiTableFlags_ScrollY
                                        | ImGuiTableFlags_RowBg
                                        | ImGuiTableFlags_BordersV
                                        | ImGuiTableFlags_Reorderable
                                        | ImGuiTableFlags_Hideable))
    {

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Effect");
        ImGui::TableSetupColumn("Remaining Time");
        ImGui::TableSetupColumn("Stacks");
        ImGui::TableSetupColumn("Prediction");
        ImGui::TableSetupColumn("Inhibited");
        ImGui::TableHeadersRow();

        static int SelectedIndex = -1;
        int Index = 0;

        const FGameplayEffectQuery Query;
        TArray<FActiveGameplayEffectHandle> Effects = AbilitySystemComponent->GetActiveEffects(Query);

        if (Config->SortByName || Config->SortByAlignment)
        {
            Effects.Sort([&](const FActiveGameplayEffectHandle& Handle1, const FActiveGameplayEffectHandle& Handle2)
            {
                const FActiveGameplayEffect* ActiveEffect1 = AbilitySystemComponent->GetActiveGameplayEffect(Handle1);
                const FActiveGameplayEffect* ActiveEffect2 = AbilitySystemComponent->GetActiveGameplayEffect(Handle2);
                const UGameplayEffect* Effect1 = ActiveEffect1 != nullptr ? ActiveEffect1->Spec.Def : nullptr;
                const UGameplayEffect* Effect2 = ActiveEffect2 != nullptr ? ActiveEffect2->Spec.Def : nullptr;
            
                bool NameOrder = false;
                if (Config->SortByName)
                {
                    const FString EffectName1 = GetEffectNameSafe(Effect1);
                    const FString EffectName2 = GetEffectNameSafe(Effect2);
                    NameOrder = EffectName1.Compare(EffectName2) < 0;
                    if (Config->SortByAlignment == false)
                    {
                        return NameOrder;   
                    }
                }

                if (Config->SortByAlignment && Asset != nullptr)
                {
                    const FGameplayTagContainer& Tags1 = Effect1->GetAssetTags();
                    const FGameplayTagContainer& Tags2 = Effect2->GetAssetTags();
                    const int32 Pos1 = Tags1.HasTag(Asset->PositiveEffectTag) ? 1 : Tags1.HasTag(Asset->NegativeEffectTag) ? -1 : 0;
                    const int32 Pos2 = Tags2.HasTag(Asset->PositiveEffectTag) ? 1 : Tags2.HasTag(Asset->NegativeEffectTag) ? -1 : 0;
                    const int32 Diff = Pos2 - Pos1;
                    if (Diff == 0)
                    {
                        return NameOrder;
                    }

                    return Diff < 0;
                }

                return false;
            });
        }

        for (const FActiveGameplayEffectHandle& ActiveHandle : Effects)
        {
            ImGui::PushID(Index);
            RenderEffectRow(*AbilitySystemComponent, ActiveHandle, Index, SelectedIndex);
            ImGui::PopID();
            Index++;
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderEffectRow(UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffectHandle& ActiveHandle, int32 Index, int32& Selected)
{
    const FActiveGameplayEffect* ActiveEffectPtr = AbilitySystemComponent.GetActiveGameplayEffect(ActiveHandle);
    if (ActiveEffectPtr == nullptr)
    {
        return;
    }

    const UGameplayEffect* EffectPtr = ActiveEffectPtr->Spec.Def;
    if (EffectPtr == nullptr)
    {
        return;
    }

    const FActiveGameplayEffect& ActiveEffect = *ActiveEffectPtr;
    const UGameplayEffect& Effect = *EffectPtr;

    const auto EffectName = StringCast<ANSICHAR>(*GetEffectName(Effect));
    if (Filter.PassFilter(EffectName.Get()) == false)
    {
        return;
    }


    ImGui::TableNextRow(ImGuiTableRowFlags_None, 0.0f);

    //------------------------
    // Name
    //------------------------
    ImGui::TableNextColumn();

    FLinearColor Color;
    AlignmentConfig->GetEffectColor(Asset, Effect, Color);
    ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(Color));

    if (ImGui::Selectable(EffectName.Get(), Selected == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
    {
        Selected = Index;
    }

    ImGui::PopStyleColor(1);

    //------------------------
    // Popup
    //------------------------
    if (FCogWidgets::BeginItemTableTooltip())
    {
        RenderEffectInfo(AbilitySystemComponent, ActiveEffect, Effect);
        FCogWidgets::EndItemTableTooltip();
    }

    //------------------------
    // ContextMenu
    //------------------------
    if (ImGui::BeginPopupContextItem())
    {
        const ImVec2 ButtonsSize = ImVec2(ImGui::GetFontSize() * 10, 0);

        bool bOpen = OpenedEffects.Contains(ActiveHandle);
        if (ImGui::Checkbox("Open Details", &bOpen))
        {
            if (bOpen)
            {
                OpenEffect(ActiveHandle);
            }
            else
            {
                CloseEffect(ActiveHandle);
            }
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Remove", ButtonsSize))
        {
            AbilitySystemComponent.RemoveActiveGameplayEffect(ActiveHandle);
            ImGui::CloseCurrentPopup();
        }

        FCogWidgets::OpenObjectAssetButton(EffectPtr, ButtonsSize);

        ImGui::EndPopup();
    }

    //------------------------
    // Remaining Time
    //------------------------
    ImGui::TableNextColumn();
    RenderRemainingTime(AbilitySystemComponent, ActiveEffect);

    //------------------------
    // Stacks
    //------------------------
    ImGui::TableNextColumn();
    RenderStacks(ActiveEffect, Effect);

    //------------------------
    // Prediction
    //------------------------
    ImGui::TableNextColumn();
    RenderPrediction(ActiveEffect, true);

    //------------------------
    // Inhibited
    //------------------------
    ImGui::TableNextColumn();
    RenderInhibition(ActiveEffect, true);

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderEffectInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect)
{
    if (ImGui::BeginTable("Effect", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {
        constexpr ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        //------------------------
        // Name
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Name");
        ImGui::TableNextColumn();
        ImGui::Text("%s", TCHAR_TO_ANSI(*GetEffectName(Effect)));

        //------------------------
        // Level
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Level");
        ImGui::TableNextColumn();
        ImGui::Text("%0.0f", ActiveEffect.Spec.GetLevel());

        //------------------------
        // Remaining Time
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Remaining Time");
        ImGui::TableNextColumn();
        RenderRemainingTime(AbilitySystemComponent, ActiveEffect);

        //------------------------
        // Period
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Period");
        ImGui::TableNextColumn();
        ImGui::Text("%0.3f", ActiveEffect.GetPeriod());

        //------------------------
        // Stacks
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Stacks");
        ImGui::TableNextColumn();
        RenderStacks(ActiveEffect, Effect);

        //------------------------
        // Prediction
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Prediction");
        ImGui::TableNextColumn();
        RenderPrediction(ActiveEffect, false);

        //------------------------
        // Inhibited
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Inhibited");
        ImGui::TableNextColumn();
        RenderInhibition(ActiveEffect, false);

        //------------------------
        // Dynamic Asset Tags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Dynamic Asset Tags");
        ImGui::TableNextColumn();
        FCogAbilityHelper::RenderTagContainer(ActiveEffect.Spec.GetDynamicAssetTags());

        //------------------------
        // All Asset Tags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "All Asset Tags");
        ImGui::TableNextColumn();
        FGameplayTagContainer AllAssetTagsContainer;
        ActiveEffect.Spec.GetAllAssetTags(AllAssetTagsContainer);
        FCogAbilityHelper::RenderTagContainer(AllAssetTagsContainer);

        //------------------------
        // All Granted Tags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "All Granted Tags");
        ImGui::TableNextColumn();
        FGameplayTagContainer AllGrantedTagsContainer;
        ActiveEffect.Spec.GetAllGrantedTags(AllGrantedTagsContainer);
        FCogAbilityHelper::RenderTagContainer(AllGrantedTagsContainer);

        //------------------------
        // Modifiers
        //------------------------
        for (int32 i = 0; i < ActiveEffect.Spec.Modifiers.Num(); ++i)
        {
            const FModifierSpec& ModSpec = ActiveEffect.Spec.Modifiers[i];
            const FGameplayModifierInfo& ModInfo = ActiveEffect.Spec.Def->Modifiers[i];
            const float AttributeBaseValue = AbilitySystemComponent.GetNumericAttributeBase(ModInfo.Attribute);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(TextColor, "Attribute Modifier");
            ImGui::TextColored(TextColor, "Operation");
            ImGui::TextColored(TextColor, "Magnitude");
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(*ModInfo.Attribute.GetName()));
            ImGui::Text("%s", TCHAR_TO_ANSI(*EGameplayModOpToString(ModInfo.ModifierOp)));
            ImGui::TextColored(FCogImguiHelper::ToImVec4(AlignmentConfig->GetEffectModifierColor(ModSpec, ModInfo, AttributeBaseValue)), "%0.2f", ModSpec.GetEvaluatedMagnitude());
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogAbilityWindow_Effects::GetEffectName(const UGameplayEffect& Effect)
{
    FString Str = FCogAbilityHelper::CleanupName(Effect.GetName());
    return Str;
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogAbilityWindow_Effects::GetEffectNameSafe(const UGameplayEffect* Effect)
{
    if (Effect == nullptr)
    {
        return "none";
    }

    return GetEffectName(*Effect);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderRemainingTime(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect)
{
    const float StartTime = ActiveEffect.StartWorldTime;
    const float Duration = ActiveEffect.GetDuration();

    if (Duration >= 0)
    {
        const UWorld* World = AbilitySystemComponent.GetWorld();
        const float RemainingTime = StartTime + Duration - World->GetTimeSeconds();

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(100, 100, 100, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 100));
        ImGui::ProgressBar(RemainingTime / Duration, ImVec2(-1, ImGui::GetTextLineHeightWithSpacing() * 0.8f), TCHAR_TO_ANSI(*FString::Printf(TEXT("%.2f / %.2f"), RemainingTime, Duration)));
        ImGui::PopStyleColor(2);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderStacks(const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect)
{
    const int32 CurrentStackCount = ActiveEffect.Spec.GetStackCount();
    if (Effect.StackLimitCount <= 0)
    {
        ImGui::Text("0");
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(100, 100, 100, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 100));
        ImGui::SetNextItemWidth(-1);
        ImGui::ProgressBar(CurrentStackCount / (float)Effect.StackLimitCount, ImVec2(FCogWidgets::GetFontWidth() * 15, ImGui::GetTextLineHeightWithSpacing() * 0.8f), TCHAR_TO_ANSI(*FString::Printf(TEXT("%d / %d"), CurrentStackCount, Effect.StackLimitCount)));
        ImGui::PopStyleColor(2);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderPrediction(const FActiveGameplayEffect& ActiveEffect, bool Short)
{
    FString PredictionString;
    if (ActiveEffect.PredictionKey.IsValidKey())
    {
        if (ActiveEffect.PredictionKey.WasLocallyGenerated())
        {
            PredictionString = Short ? FString("Wait") : FString("Predicted and Waiting");
        }
        else
        {
            PredictionString = Short ? FString("Done") : FString("Predicted and Caught Up");
        }
    }
    else
    {
        PredictionString = Short ? FString("No") : FString("Not Predicted");
    }

    ImGui::Text("%s", TCHAR_TO_ANSI(*PredictionString));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderInhibition(const FActiveGameplayEffect& ActiveEffect, bool Short)
{
    if (ActiveEffect.bIsInhibited)
    {
        FCogWidgets::SmallButton("Yes", ImVec4(1, 0, 0, 1));
    }
    else
    {
        ImGui::Text("No");
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::OpenEffect(const FActiveGameplayEffectHandle& Handle)
{
    OpenedEffects.AddUnique(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::CloseEffect(const FActiveGameplayEffectHandle& Handle)
{
    OpenedEffects.Remove(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Effects::RenderOpenEffects()
{
    const AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Selection, true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    for (int i = OpenedEffects.Num() - 1; i >= 0; --i)
    {
        const FActiveGameplayEffectHandle Handle = OpenedEffects[i];

        const FActiveGameplayEffect* ActiveEffectPtr = AbilitySystemComponent->GetActiveGameplayEffect(Handle);
        if (ActiveEffectPtr == nullptr)
        {
            return;
        }

        const UGameplayEffect* Effect = ActiveEffectPtr->Spec.Def;
        if (Effect == nullptr)
        {
            return;
        }

        bool Open = true;
        if (ImGui::Begin(TCHAR_TO_ANSI(*GetEffectName(*Effect)), &Open))
        {
            RenderEffectInfo(*AbilitySystemComponent, *ActiveEffectPtr, *Effect);
            ImGui::End();
        }

        if (Open == false)
        {
            OpenedEffects.RemoveAt(i);
        }
    }
}
