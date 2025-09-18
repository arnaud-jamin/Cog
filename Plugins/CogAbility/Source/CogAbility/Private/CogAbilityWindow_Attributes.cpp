#include "CogAbilityWindow_Attributes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityConfig_Alignment.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "AttributeSet.h"
#include "CogAbilityWindow_Abilities.h"
#include "CogCommon.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogAbilityConfig_Attributes>();
    AlignmentConfig = GetConfig<UCogAbilityConfig_Alignment>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderHelp()
{
    ImGui::Text(
        "This window displays the gameplay attributes of the selected actor. "
        "Attributes can be sorted by name, category or attribute set. "
        "Attributes with the Current value greater than the Base value are displayed in green. "
        "Attributes with the Current value lower than the Base value are displayed in red. "
        "Use the options 'Show Only Modified' to only show the attributes that have modifiers. "
        );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::PostBegin()
{
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    RenderOpenAttributes();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::FormatAttributeSetName(FString& AttributeSetName)
{
    if (Config->AttributeSetPrefixes.IsEmpty() == false)
    {
        TArray<FString> Prefixes;
        Config->AttributeSetPrefixes.ParseIntoArray(Prefixes, TEXT(";"));

        for (const FString& Prefix : Prefixes)
        {
            if (AttributeSetName.RemoveFromStart(Prefix, ESearchCase::IgnoreCase))
            {
                break;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderContent()
{
    Super::RenderContent();

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        ImGui::TextDisabled("Selection has no ability system component");
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Sort by Name", &Config->SortByName);
            ImGui::Checkbox("Group by Attribute Set", &Config->GroupByAttributeSet);
            ImGui::Checkbox("Group by Category", &Config->GroupByCategory);
            ImGui::Checkbox("Show Only Modified", &Config->ShowOnlyModified);

            FCogWidgets::SetNextItemToShortWidth();
            FCogWidgets::InputText("Attribute Set Prefixes", Config->AttributeSetPrefixes);
            ImGui::SetItemTooltip("Prefixes to remove from the attribute set name. Separate multiple prefixes with the semicolon character ';'");

            ImGui::Separator();
            ImGui::ColorEdit4("Positive Color", &AlignmentConfig->PositiveColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Negative Color", &AlignmentConfig->NegativeColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Neutral Color", &AlignmentConfig->NeutralColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("AttributeSet Color", &Config->AttributeSetColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Category Color", &Config->CategoryColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
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

    const bool bGroupByAttributeSetValue = Filter.IsActive() == false && Config->ShowOnlyModified == false && Config->GroupByAttributeSet;
    const bool bGroupByCategoryValue = Filter.IsActive() == false && Config->ShowOnlyModified == false && Config->GroupByCategory;
    const float bShowGroup = bGroupByAttributeSetValue | bGroupByCategoryValue;
    const float FirstColWidth = (static_cast<int32>(bGroupByAttributeSetValue) + static_cast<int32>(bGroupByCategoryValue)) * ImGui::GetFontSize() * 2;

    if (ImGui::BeginTable("Attributes", 5, ImGuiTableFlags_SizingFixedFit 
                                         | ImGuiTableFlags_Resizable 
                                         | ImGuiTableFlags_NoBordersInBodyUntilResize 
                                         | ImGuiTableFlags_ScrollY 
                                         | ImGuiTableFlags_RowBg 
                                         | ImGuiTableFlags_BordersV 
                                         | ImGuiTableFlags_Reorderable 
                                         | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("-", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, FirstColWidth);
        ImGui::TableSetupColumn("Set", bShowGroup ? ImGuiTableColumnFlags_DefaultHide : ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Attribute");
        ImGui::TableSetupColumn("Base");
        ImGui::TableSetupColumn("Current");
        ImGui::TableHeadersRow();

        static int Selected = -1;
        int Index = 0;

        //------------------------------------------------------------------------------------------
        // Draw all the attribute sets
        //------------------------------------------------------------------------------------------
        for (const UAttributeSet* AttributeSet : AbilitySystemComponent->GetSpawnedAttributes())
        {
            if (AttributeSet == nullptr)
            { continue; }

            ImGui::PushID(COG_TCHAR_TO_CHAR(*AttributeSet->GetName()));

            FString AttributeSetName = AttributeSet->GetName();
            FormatAttributeSetName(AttributeSetName);
            const auto AttributeSetNameStr = StringCast<ANSICHAR>(*AttributeSetName);

            //------------------------------------------------------------------------------------------
            // Add a tree node with the name of the attribute set if grouping by attribute set 
            //------------------------------------------------------------------------------------------
            bool bOpenAttributeSet = true;
            if (bGroupByAttributeSetValue)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(Config->AttributeSetColor));
                bOpenAttributeSet = ImGui::TreeNodeEx(AttributeSetNameStr.Get(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_LabelSpanAllColumns);
                ImGui::PopStyleColor();

            }

            if (bOpenAttributeSet)
            {
                TArray<FGameplayAttribute> AllAttributes;
                for (TFieldIterator<FProperty> It(AttributeSet->GetClass()); It; ++It)
                {
                    FGameplayAttribute Attribute = *It;
                    if (Attribute.IsValid())
                    {
                        AllAttributes.Add(Attribute);
                    }
                }

                TMap<FString, TArray<FGameplayAttribute>> AttributesByCategory;

                //------------------------------------------------------------------------------------------
                // Sort attributes by category to make sure categories are displayed in alphabetical order
                //------------------------------------------------------------------------------------------
#if WITH_EDITORONLY_DATA
                if (bGroupByCategoryValue)
                {
                    AllAttributes.Sort([](const FGameplayAttribute& Attribute1, const FGameplayAttribute& Attribute2)
                        {
                            FString Category1 = Attribute1.GetUProperty() != nullptr ? Attribute1.GetUProperty()->GetMetaData(TEXT("Category")) : "";
                            FString Category2 = Attribute2.GetUProperty() != nullptr ? Attribute2.GetUProperty()->GetMetaData(TEXT("Category")) : "";
                            return Category1.Compare(Category2) < 0;
                        });
                }
#endif //WITH_EDITORONLY_DATA

                //------------------------------------------------------------------------------------------
                // If required, group by category, or put every attribute into the default category
                // if this code is changed verify the SortByName works in all modes
                //------------------------------------------------------------------------------------------
                for (FGameplayAttribute& Attribute : AllAttributes)
                {
                    FString Category = TEXT("Default");

#if WITH_EDITORONLY_DATA
                    if (bGroupByCategoryValue)
                    {
                        FString ActualCategory = Attribute.GetUProperty() != nullptr ? Attribute.GetUProperty()->GetMetaData(TEXT("Category")) : "";
                        if (ActualCategory.IsEmpty() == false)
                        {
                            Category = ActualCategory;
                        }
                    }
#endif //WITH_EDITORONLY_DATA

                    AttributesByCategory.FindOrAdd(Category).Add(Attribute);
                }

                for (auto& It : AttributesByCategory)
                {
                    //------------------------------------------------------------------------------------------
                    // Add a tree node with the name of the category if categories are shown
                    //------------------------------------------------------------------------------------------
                    bool bOpenCategory = true;
                    if (bGroupByCategoryValue)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(Config->CategoryColor));
                        bOpenCategory = ImGui::TreeNodeEx(COG_TCHAR_TO_CHAR(*It.Key), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_LabelSpanAllColumns);
                        ImGui::PopStyleColor();
                    }

                    if (bOpenCategory)
                    {
                        TArray<FGameplayAttribute>& AttributesInCategory = It.Value;

                        //------------------------------------------------------------------------------------------
                        // Sort attributes within a category by their name
                        //------------------------------------------------------------------------------------------
                        if (Config->SortByName)
                        {
                            AttributesInCategory.Sort([](const FGameplayAttribute& Lhs, const FGameplayAttribute& Rhs)
                                {
                                    return Lhs.GetName().Compare(Rhs.GetName()) < 0;
                                });
                        }

                        //------------------------------------------------------------------------------------------
                        // Draw all the attribute of the current category of the current attribute set
                        //------------------------------------------------------------------------------------------

                        for (const FGameplayAttribute& Attribute : AttributesInCategory)
                        {
                            if (!Attribute.IsValid())
                            { continue; }

                            const auto AttributeNameStr = StringCast<ANSICHAR>(*Attribute.GetName());

                            if (Filter.PassFilter(AttributeNameStr.Get()) == false)
                            { continue; }

                            const float BaseValue = AbilitySystemComponent->GetNumericAttributeBase(Attribute);
                            const float CurrentValue = AbilitySystemComponent->GetNumericAttribute(Attribute);

                            if (Config->ShowOnlyModified && FMath::IsNearlyEqual(CurrentValue, BaseValue))
                            { continue; }

                            ImGui::PushID(AttributeNameStr.Get());

                            ImGui::TableNextRow();

                            //------------------------
                            // Selectable
                            //------------------------
                            ImGui::TableNextColumn();
                            if (ImGui::Selectable("", Selected == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick))
                            {
                                Selected = Index;

                                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                {
                                    OpenAttributeDetails(Attribute);
                                }
                            }

                            //------------------------
                            // Popup
                            //------------------------
                            if (FCogWidgets::BeginItemTableTooltip())
                            {
                                RenderAttributeDetails(*AbilitySystemComponent, AttributeSetNameStr.Get(), Attribute, true);
                                FCogWidgets::EndItemTableTooltip();
                            }

                            //------------------------
                            // ContextMenu
                            //------------------------
                            RenderAttributeContextMenu(*AbilitySystemComponent, Attribute, Index);

                            const ImVec4 Color = FCogImguiHelper::ToImVec4(AlignmentConfig->GetAttributeColor(*AbilitySystemComponent, Attribute));
                            ImGui::PushStyleColor(ImGuiCol_Text, Color);

                            //------------------------
                            // Attribute Set
                            //------------------------
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", AttributeSetNameStr.Get());

                            //------------------------
                            // Attribute  Name
                            //------------------------
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", AttributeNameStr.Get());
                            ImGui::PopStyleColor(1);

                            ImGui::PushStyleColor(ImGuiCol_Text, Color);

                            //------------------------
                            // Base Value
                            //------------------------
                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", BaseValue);

                            //------------------------
                            // Current Value
                            //------------------------
                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", CurrentValue);

                            ImGui::PopStyleColor(1);

                            ImGui::PopID();

                            Index++;
                        }
                    }

                    if (bOpenCategory && bGroupByCategoryValue)
                    {
                        ImGui::TreePop();
                    }
                }
            }

            if (bGroupByAttributeSetValue)
            {
                if (bOpenAttributeSet)
                {
                    ImGui::TreePop();
                }
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderAttributeDetails(const UAbilitySystemComponent& AbilitySystemComponent, const char* AttributeSetName, const FGameplayAttribute& Attribute, bool IsForTooltip)
{
    ImGuiTableFlags TableFlags = IsForTooltip ? ImGuiTableFlags_Borders
                                              : ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_Resizable;

    const float BaseValue = AbilitySystemComponent.GetNumericAttributeBase(Attribute);
    const float CurrentValue = AbilitySystemComponent.GetNumericAttribute(Attribute);

    if (ImGui::BeginTable("Details", 2, TableFlags))
    {
        constexpr ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        if (IsForTooltip == false)
        {
            ImGui::TableHeadersRow();
        }

        //------------------------
        // Attribute Set
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Attribute Set");
        ImGui::TableNextColumn();
        ImGui::Text("%s", AttributeSetName);

        //------------------------
        // Name
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Name");
        ImGui::TableNextColumn();
        ImGui::Text("%s", COG_TCHAR_TO_CHAR(*Attribute.GetName()));

        //------------------------
        // Base Value
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Base Value");
        ImGui::TableNextColumn();
        ImGui::Text("%0.2f", BaseValue);

        //------------------------
        // Current Value
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Current Value");
        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(AlignmentConfig->GetAttributeColor(AbilitySystemComponent, Attribute)));
        ImGui::Text("%0.2f", CurrentValue);
        ImGui::PopStyleColor(1);

        ImGui::EndTable();
    }

    //------------------------
    // Modifiers
    //------------------------
    int32 ModifierIndex = 1;

    const FGameplayEffectQuery Query;
    for (const FActiveGameplayEffectHandle& ActiveHandle : AbilitySystemComponent.GetActiveEffects(Query))
    {
        const FActiveGameplayEffect* ActiveEffect = AbilitySystemComponent.GetActiveGameplayEffect(ActiveHandle);
        if (ActiveEffect == nullptr)
        {
            continue;
        }

        for (int32 i = 0; i < ActiveEffect->Spec.Modifiers.Num(); ++i)
        {
            const FModifierSpec& ModSpec = ActiveEffect->Spec.Modifiers[i];
            const FGameplayModifierInfo& ModInfo = ActiveEffect->Spec.Def->Modifiers[i];

            if (ModInfo.Attribute == Attribute)
            {
                char Buffer[128];
                ImFormatString(Buffer, IM_ARRAYSIZE(Buffer), "Modifier %d", ModifierIndex);

                if (FCogWidgets::DarkCollapsingHeader(Buffer, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::BeginTable("Details", 2, TableFlags))
                    {
                        constexpr ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

                        ImGui::TableSetupColumn("Property");
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(TextColor, "Effect");
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", COG_TCHAR_TO_CHAR(*FCogAbilityHelper::CleanupName(GetNameSafe(ActiveEffect->Spec.Def))));

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(TextColor, "Operation");
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", COG_TCHAR_TO_CHAR(*EGameplayModOpToString(ModInfo.ModifierOp)));

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(TextColor, "Magnitude");
                        ImGui::TableNextColumn();
                        ImGui::TextColored(FCogImguiHelper::ToImVec4(AlignmentConfig->GetEffectModifierColor(ModSpec, ModInfo, BaseValue)), "%0.2f", ModSpec.GetEvaluatedMagnitude());

                        if (ModInfo.SourceTags.RequireTags.IsEmpty() == false)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(TextColor, "SourceTags - Require");
                            ImGui::TableNextColumn();
                            FCogAbilityHelper::RenderTagContainer(ModInfo.SourceTags.RequireTags, false);
                        }

                        if (ModInfo.SourceTags.IgnoreTags.IsEmpty() == false)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(TextColor, "SourceTags - Ignore");
                            ImGui::TableNextColumn();
                            FCogAbilityHelper::RenderTagContainer(ModInfo.SourceTags.IgnoreTags, false);
                        }

                        if (ModInfo.SourceTags.TagQuery.IsEmpty() == false)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(TextColor, "SourceTags - Query");
                            ImGui::TableNextColumn();
                            const auto Str = StringCast<ANSICHAR>(*ModInfo.SourceTags.TagQuery.GetDescription());
                            ImGui::Text("%s", Str.Get());
                        }

                        if (ModInfo.TargetTags.RequireTags.IsEmpty() == false)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(TextColor, "TargetTags - Require");
                            ImGui::TableNextColumn();
                            FCogAbilityHelper::RenderTagContainer(ModInfo.TargetTags.RequireTags, false);
                        }

                        if (ModInfo.TargetTags.IgnoreTags.IsEmpty() == false)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(TextColor, "TargetTags - Ignore");
                            ImGui::TableNextColumn();
                            FCogAbilityHelper::RenderTagContainer(ModInfo.TargetTags.IgnoreTags, false);
                        }

                        if (ModInfo.TargetTags.TagQuery.IsEmpty() == false)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextColored(TextColor, "TargetTags - Query");
                            ImGui::TableNextColumn();
                            const auto Str = StringCast<ANSICHAR>(*ModInfo.TargetTags.TagQuery.GetDescription());
                            ImGui::Text("%s", Str.Get());
                        }

                        ImGui::EndTable();
                    }
                }

                ModifierIndex++;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderOpenAttributes()
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

    for (int i = OpenedAttributes.Num() - 1; i >= 0; --i)
    {
        const FGameplayAttribute& Attribute = OpenedAttributes[i];

        FString AttributeSetName = Attribute.GetAttributeSetClass()->GetFName().ToString();
        FormatAttributeSetName(AttributeSetName);

        const FString WindowName = AttributeSetName + FString(" - ") + Attribute.GetName();
        const auto WindowNameStr = StringCast<ANSICHAR>(*WindowName);

        bool Open = true;
        if (ImGui::Begin(WindowNameStr.Get(), &Open))
        {
            const auto AttributeSetNameStr = StringCast<ANSICHAR>(*AttributeSetName);
            RenderAttributeDetails(*AbilitySystemComponent, AttributeSetNameStr.Get(), Attribute, false);
            ImGui::End();
        }

        if (Open == false)
        {
            OpenedAttributes.RemoveAt(i);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderAttributeContextMenu(UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& InAttribute, int Index)
{
    if (ImGui::BeginPopupContextItem())
    {
        bool bOpen = OpenedAttributes.Contains(InAttribute);
        if (ImGui::Checkbox("Open Details", &bOpen))
        {
            if (bOpen)
            {
                OpenAttributeDetails(InAttribute);
            }
            else
            {
                CloseAttributeDetails(InAttribute);
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::OpenAttributeDetails(const FGameplayAttribute& InAttribute)
{
    OpenedAttributes.AddUnique(InAttribute);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::CloseAttributeDetails(const FGameplayAttribute& InAttribute)
{
    OpenedAttributes.Remove(InAttribute);
}