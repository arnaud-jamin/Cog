#include "CogAbilityWindow_Attributes.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityConfig_Alignment.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogImguiHelper.h"
#include "CogWindowHelper.h"
#include "CogWindowWidgets.h"
#include "AttributeSet.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

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
void FCogAbilityWindow_Attributes::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::RenderContent()
{
    Super::RenderContent();

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
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

        FCogWindowWidgets::MenuSearchBar(Filter);

        ImGui::EndMenuBar();
    }

    bool bGroupByAttributeSetValue = Filter.IsActive() == false && Config->ShowOnlyModified == false && Config->GroupByAttributeSet;
    bool bGroupByCategoryValue = Filter.IsActive() == false && Config->ShowOnlyModified == false && Config->GroupByCategory;

    if (ImGui::BeginTable("Attributes", 3, ImGuiTableFlags_SizingFixedFit 
                                         | ImGuiTableFlags_Resizable 
                                         | ImGuiTableFlags_NoBordersInBodyUntilResize 
                                         | ImGuiTableFlags_ScrollY 
                                         | ImGuiTableFlags_RowBg 
                                         | ImGuiTableFlags_BordersOuter 
                                         | ImGuiTableFlags_BordersV 
                                         | ImGuiTableFlags_Reorderable 
                                         | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Attribute");
        ImGui::TableSetupColumn("Base");
        ImGui::TableSetupColumn("Current");
        ImGui::TableHeadersRow();

        static int Selected = -1;
        int Index = 0;

        //------------------------------------------------------------------------------------------
        // Draw all the attribute sets
        //------------------------------------------------------------------------------------------
        for (const UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
        {
            //------------------------------------------------------------------------------------------
            // Add an tree node categories are shown
            //------------------------------------------------------------------------------------------
            bool bOpenAttributeSet = true;
            if (bGroupByAttributeSetValue)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bOpenAttributeSet = ImGui::TreeNodeEx(TCHAR_TO_ANSI(*Set->GetName()), ImGuiTreeNodeFlags_SpanFullWidth);
            }

            if (bOpenAttributeSet)
            {
                TArray<FGameplayAttribute> AllAttributes;
                for (TFieldIterator<FProperty> It(Set->GetClass()); It; ++It)
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
                        bOpenCategory = ImGui::TreeNodeEx(TCHAR_TO_ANSI(*It.Key), ImGuiTreeNodeFlags_SpanFullWidth);
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
                            {
                                continue;
                            }

                            const char* AttributeName = TCHAR_TO_ANSI(*Attribute.GetName());

                            if (Filter.PassFilter(AttributeName) == false)
                            {
                                continue;
                            }

                            const float BaseValue = AbilitySystemComponent->GetNumericAttributeBase(Attribute);
                            const float CurrentValue = AbilitySystemComponent->GetNumericAttribute(Attribute);

                            if (Config->ShowOnlyModified && FMath::IsNearlyEqual(CurrentValue, BaseValue))
                            {
                                continue;
                            }

                            ImGui::TableNextRow();

                            const ImVec4 Color = FCogImguiHelper::ToImVec4(AlignmentConfig->GetAttributeColor(*AbilitySystemComponent, Attribute));
                            ImGui::PushStyleColor(ImGuiCol_Text, Color);

                            //------------------------
                            // Name
                            //------------------------
                            ImGui::TableNextColumn();
                            ImGui::Text("");
                            ImGui::SameLine();
                            if (ImGui::Selectable(AttributeName, Selected == Index, ImGuiSelectableFlags_SpanAllColumns))
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
                                DrawAttributeInfo(*AbilitySystemComponent, Attribute);
                                FCogWindowWidgets::EndTableTooltip();
                            }

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

                            Index++;
                        }
                    }

                    if (bOpenCategory && bGroupByCategoryValue)
                    {
                        ImGui::TreePop();
                    }
                }
            }

            if (bOpenAttributeSet && bGroupByAttributeSetValue)
            {
                ImGui::TreePop();
            }
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Attributes::DrawAttributeInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute)
{
    if (ImGui::BeginTable("Attribute", 2, ImGuiTableFlags_Borders))
    {
        const ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        const float BaseValue = AbilitySystemComponent.GetNumericAttributeBase(Attribute);
        const float CurrentValue = AbilitySystemComponent.GetNumericAttribute(Attribute);

        //------------------------
        // Name
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Name");
        ImGui::TableNextColumn();
        ImGui::Text("%s", TCHAR_TO_ANSI(*Attribute.GetName()));

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

        //------------------------
        // Modifiers
        //------------------------
        FGameplayEffectQuery Query;
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
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(TextColor, "Effect");
                    ImGui::TextColored(TextColor, "Operation");
                    ImGui::TextColored(TextColor, "Magnitude");
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", TCHAR_TO_ANSI(*FCogAbilityHelper::CleanupName(GetNameSafe(ActiveEffect->Spec.Def))));
                    ImGui::Text("%s", TCHAR_TO_ANSI(*EGameplayModOpToString(ModInfo.ModifierOp)));
                    ImGui::TextColored(FCogImguiHelper::ToImVec4(AlignmentConfig->GetEffectModifierColor(ModSpec, ModInfo, BaseValue)), "%0.2f", ModSpec.GetEvaluatedMagnitude());
                }
            }
        }

        ImGui::EndTable();
    }
}
