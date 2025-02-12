#include "CogAbilityWindow_Tags.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityHelper.h"
#include "CogWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderHelp()
{
    ImGui::Text("This window displays gameplay tags of the selected actor. ");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::PostBegin()
{
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderContent()
{
    Super::RenderContent();

    RenderMenu();

    const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        ImGui::TextDisabled("Selection has no ability system component");
        return;
    }

    FGameplayTagContainer Tags;
    GetTagContainer(Tags);

    RenderTagContainer(*AbilitySystemComponent, Tags);
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

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tags::RenderTagContainer(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer)
{
    if (ImGui::BeginTable("Tags", 2, ImGuiTableFlags_SizingFixedFit
                                                                | ImGuiTableFlags_Resizable
                                                                | ImGuiTableFlags_NoBordersInBodyUntilResize
                                                                | ImGuiTableFlags_ScrollY
                                                                | ImGuiTableFlags_RowBg
                                                                | ImGuiTableFlags_BordersV
                                                                | ImGuiTableFlags_Reorderable
                                                                | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Count");
        ImGui::TableSetupColumn(TCHAR_TO_ANSI(*GetName()));
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
            if (ImGui::Selectable(TCHAR_TO_ANSI(*Tag.GetTagName().ToString()), Selected == Index, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
            {
                Selected = Index;
            }

            //------------------------
            // Tooltip
            //------------------------
            if (FCogWidgets::BeginItemTableTooltip())
            {
                RenderTag(AbilitySystemComponent, Tag);
                FCogWidgets::EndItemTableTooltip();
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

//--------------------------------------------------------------------------------------------------------------------------
// FCogAbilityWindow_OwnedTags
//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_OwnedTags::Initialize()
{
    Super::Initialize();
    Config = GetConfig<UCogAbilityConfig_OwnedTags>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_OwnedTags::RenderHelp()
{
    ImGui::Text("This window displays gameplay tags of the selected actor. ");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_OwnedTags::GetTagContainer(FGameplayTagContainer& TagContainer)
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogAbilityWindow_BlockedTags
//------------------------------------------
void FCogAbilityWindow_BlockedTags::Initialize()
{
    Super::Initialize();
    Config = GetConfig<UCogAbilityConfig_BlockedAbilitiesTags>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_BlockedTags::RenderHelp()
{
    ImGui::Text("This window displays the tags blocking abilities of the selected actor.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_BlockedTags::GetTagContainer(FGameplayTagContainer& TagContainer)
{
    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystemComponent == nullptr)
    {
        return;
    }

    AbilitySystemComponent->GetBlockedAbilityTags(TagContainer);
}
