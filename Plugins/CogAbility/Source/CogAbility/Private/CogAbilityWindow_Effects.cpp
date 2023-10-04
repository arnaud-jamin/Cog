#include "CogAbilityWindow_Effects.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AttributeSet.h"
#include "CogAbilityHelper.h"
#include "CogWindowWidgets.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderHelp()
{
    ImGui::Text(
        "This window displays the gameplay effects of the selected actor. "
        "Mouse over an effect to see its details such as its modifiers and the gameplay tags it grants"
        );
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::PreRender(ImGuiWindowFlags& WindowFlags)
{
    Super::PreRender(WindowFlags);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderContent()
{
    Super::RenderContent();

    RenderEffectsTable();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderEffectsTable()
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    ImGui::BeginTable("Effects", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH | ImGuiTableFlags_NoBordersInBody);
    ImGui::TableSetupColumn("Effect");
    ImGui::TableSetupColumn("Remaining Time");
    ImGui::TableSetupColumn("Stacks");
    ImGui::TableSetupColumn("Prediction");
    ImGui::TableHeadersRow();

    static int SelectedIndex = -1;
    int Index = 0;

    FGameplayEffectQuery Query;
    for (const FActiveGameplayEffectHandle& ActiveHandle : AbilitySystemComponent->GetActiveEffects(Query))
    {
        RenderEffectRow(*AbilitySystemComponent, ActiveHandle, Index, SelectedIndex);
    }

    ImGui::EndTable();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderEffectRow(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffectHandle& ActiveHandle, int32 Index, int32& Selected)
{
    ImGui::PushID(Index);

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

    ImGui::TableNextRow(ImGuiTableRowFlags_None, 0.0f);

    //------------------------
    // Name
    //------------------------
    ImGui::TableNextColumn();

    const FGameplayTagContainer& Tags = Effect.InheritableGameplayEffectTags.CombinedTags;

    ImVec4 Color;
    if (Tags.HasTag(NegativeEffectTag))
    {
        Color = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
    }
    else if (Tags.HasTag(PositiveEffectTag))
    {
        Color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
    }
    else
    {
        Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, Color);

    if (ImGui::Selectable(TCHAR_TO_ANSI(*GetEffectName(Effect)), Selected == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick))
    {
        Selected = Index;
    }

    ImGui::PopStyleColor(1);

    //------------------------
    // Popup
    //------------------------
    if (ImGui::IsItemHovered())
    {
        FCogWindowWidgets::BeginTableTooltip();
        RenderEffectInfo(AbilitySystemComponent, ActiveEffect, Effect);
        FCogWindowWidgets::EndTableTooltip();
    }

    //------------------------
    // ContextMenu
    //------------------------
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Button("Open"))
        {
            //GetOwner()->GetPropertyGrid()->Open(EffectPtr);
            ImGui::CloseCurrentPopup();
        }
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

    ImGui::PopID();
    Index++;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderEffectInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect)
{
    if (ImGui::BeginTable("Effect", 2, ImGuiTableFlags_Borders))
    {
        const ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

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
        // Dynamic Asset Tags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Dynamic Asset Tags");
        ImGui::TableNextColumn();
        RenderTagContainer(ActiveEffect.Spec.GetDynamicAssetTags());

        //------------------------
        // All Asset Tags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "All Asset Tags");
        ImGui::TableNextColumn();
        FGameplayTagContainer AllAssetTagsContainer;
        ActiveEffect.Spec.GetAllAssetTags(AllAssetTagsContainer);
        RenderTagContainer(AllAssetTagsContainer);

        //------------------------
        // All Granted Tags
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "All Granted Tags");
        ImGui::TableNextColumn();
        FGameplayTagContainer AllGrantedTagsContainer;
        ActiveEffect.Spec.GetAllGrantedTags(AllGrantedTagsContainer);
        RenderTagContainer(AllGrantedTagsContainer);

        //------------------------
        // Modifiers
        //------------------------
        for (int32 i = 0; i < ActiveEffect.Spec.Modifiers.Num(); ++i)
        {
            const FModifierSpec& ModSpec = ActiveEffect.Spec.Modifiers[i];
            const FGameplayModifierInfo& ModInfo = ActiveEffect.Spec.Def->Modifiers[i];

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(TextColor, "Attribute Modifier");
            ImGui::TextColored(TextColor, "Operation");
            ImGui::TextColored(TextColor, "Magnitude");
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(*ModInfo.Attribute.GetName()));
            ImGui::Text("%s", TCHAR_TO_ANSI(*EGameplayModOpToString(ModInfo.ModifierOp)));
            ImGui::Text("%0.2f", ModSpec.GetEvaluatedMagnitude());
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderTagContainer(const FGameplayTagContainer& Container)
{
    TArray<FGameplayTag> GameplayTags;
    Container.GetGameplayTagArray(GameplayTags);
    for (FGameplayTag Tag : GameplayTags)
    {
        ImGui::Text("%s", TCHAR_TO_ANSI(*Tag.ToString()));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogAbilityWindow_Effects::GetEffectName(const UGameplayEffect& Effect)
{
    FString Str = FCogAbilityHelper::CleanupName(Effect.GetName());
    return Str;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderRemainingTime(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect)
{
    float StartTime = ActiveEffect.StartWorldTime;
    float Duration = ActiveEffect.GetDuration();

    if (Duration <= 0)
    {
        ImGui::Text("NA");
    }
    else
    {
        UWorld* World = AbilitySystemComponent.GetWorld();
        const float RemainingTime = StartTime + Duration - World->GetTimeSeconds();

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(100, 100, 100, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 100));
        ImGui::ProgressBar(RemainingTime / Duration, ImVec2(FCogWindowWidgets::GetFontWidth() * 15, ImGui::GetTextLineHeightWithSpacing() * 0.8f), TCHAR_TO_ANSI(*FString::Printf(TEXT("%.2f / %.2f"), RemainingTime, Duration)));
        ImGui::PopStyleColor(2);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderStacks(const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect)
{
    const int32 CurrentStackCount = ActiveEffect.Spec.StackCount;
    if (Effect.StackLimitCount <= 0)
    {
        ImGui::Text("0");
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(100, 100, 100, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 100));
        ImGui::SetNextItemWidth(-1);
        ImGui::ProgressBar(CurrentStackCount / (float)Effect.StackLimitCount, ImVec2(FCogWindowWidgets::GetFontWidth() * 15, ImGui::GetTextLineHeightWithSpacing() * 0.8f), TCHAR_TO_ANSI(*FString::Printf(TEXT("%d / %d"), CurrentStackCount, Effect.StackLimitCount)));
        ImGui::PopStyleColor(2);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Effects::RenderPrediction(const FActiveGameplayEffect& ActiveEffect, bool Short)
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
