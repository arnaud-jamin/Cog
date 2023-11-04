#include "CogAbilityWindow_Tags.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityHelper.h"
#include "CogWindowWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogAbilityConfig_Tags>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderHelp()
{
    ImGui::Text("This window displays gameplay tags of the selected actor. ");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderContent()
{
    Super::RenderContent();

    RenderMenu();

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    FGameplayTagContainer OwnedTags, BlockedTags;
    AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
    AbilitySystemComponent->GetBlockedAbilityTags(BlockedTags);

    RenderTagContainer("Owned Tags", *AbilitySystemComponent, OwnedTags);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Sort by Name", &Config->SortByName);

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
void FCogAbilityWindow_Tags::RenderTagContainer(const FString& TagContainerName, const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer)
{
    if (ImGui::BeginTable(TCHAR_TO_ANSI(*TagContainerName), 2, ImGuiTableFlags_SizingFixedFit
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
        ImGui::TableSetupColumn("Count");
        ImGui::TableSetupColumn(TCHAR_TO_ANSI(*TagContainerName));
        ImGui::TableHeadersRow();

        TArray<FGameplayTag> Tags;
        TagContainer.GetGameplayTagArray(Tags);

        Tags.RemoveAll([&](const FGameplayTag& Tag) 
        {
            return Filter.PassFilter(TCHAR_TO_ANSI(*Tag.GetTagName().ToString())) == false; 
        });

        if (Config->SortByName)
        {
            Tags.Sort([](const FGameplayTag& Tag1, const FGameplayTag& Tag2)
            {
                return Tag1.GetTagName().Compare(Tag2.GetTagName()) < 0;
            });
        }

        static int Selected = -1;
        int Index = 0;

        for (const FGameplayTag& Tag : Tags)
        {
            ImGui::TableNextRow();

            ImGui::PushID(Index);

            //------------------------
            // Count
            //------------------------
            ImGui::TableNextColumn();
            const int32 TagCount = AbilitySystemComponent.GetTagCount(Tag);
            ImGui::TextColored(TagCount > 1 ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%d", TagCount);

            //------------------------
            // Name
            //------------------------
            ImGui::TableNextColumn();
            if (ImGui::Selectable(TCHAR_TO_ANSI(*Tag.GetTagName().ToString()), Selected == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick))
            {
                Selected = Index;
            }

            //------------------------
            // Tooltip
            //------------------------
            if (ImGui::IsItemHovered())
            {
                FCogWindowWidgets::BeginTableTooltip();
                RenderTag(AbilitySystemComponent, Tag);
                FCogWindowWidgets::EndTableTooltip();
            }

            ImGui::PopID();
            Index++;
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderTag(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTag& Tag)
{
    if (ImGui::BeginTable("Tag", 2, ImGuiTableFlags_Borders))
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
        ImGui::Text("%s", TCHAR_TO_ANSI(*Tag.GetTagName().ToString()));

        //------------------------
        // Base Value
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Count");
        ImGui::TableNextColumn();
        const int32 TagCount = AbilitySystemComponent.GetTagCount(Tag);
        ImGui::TextColored(TagCount > 1 ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%d", TagCount);

        //------------------------
        // Effects
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Effects");
        ImGui::TableNextColumn();
        FGameplayEffectQuery Query;
        for (const FActiveGameplayEffectHandle& ActiveHandle : AbilitySystemComponent.GetActiveEffects(Query))
        {
            const FActiveGameplayEffect* ActiveEffect = AbilitySystemComponent.GetActiveGameplayEffect(ActiveHandle);
            if (ActiveEffect == nullptr)
            {
                continue;
            }

            FGameplayTagContainer Container;
            ActiveEffect->Spec.GetAllGrantedTags(Container);

            if (Container.HasTagExact(Tag))
            {
                ImGui::Text("%s", TCHAR_TO_ANSI(*FCogAbilityHelper::CleanupName(GetNameSafe(ActiveEffect->Spec.Def))));
            }
        }

        ImGui::EndTable();
    }
}