#include "CogAbilityWindow_Tweaks.h"

#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogAbilityReplicator.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tweaks::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Asset = GetAsset<UCogAbilityDataAsset>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tweaks::RenderHelp()
{
    ImGui::Text(
        "This window can be used to apply tweaks to all the loaded actors. "
        "Tweaks are used to test various gameplay settings by actor category. "
    );

    FCogAbilityHelper::RenderConfigureMessage(Asset);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tweaks::RenderContent()
{
    Super::RenderContent();

    if (Asset == nullptr)
    {
        ImGui::TextDisabled("Invalid Asset");
        return;
    }

    ACogAbilityReplicator* Replicator = ACogAbilityReplicator::GetFirstReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        ImGui::TextDisabled("Invalid Replicator");
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Reset"))
        {
            Replicator->Server_ResetAllTweaks();
        }

        ImGui::EndMenuBar();
    }

    const int32 CurrentTweakProfileIndex = Replicator->GetTweakProfileIndex();
    FName CurrentProfileName = FName("None");
    if (Asset->TweakProfiles.IsValidIndex(CurrentTweakProfileIndex))
    {
        CurrentProfileName = Asset->TweakProfiles[CurrentTweakProfileIndex].Name;
    }

    if (ImGui::BeginCombo("Profile", TCHAR_TO_ANSI(*CurrentProfileName.ToString())))
    {
        {
	        const bool IsSelected = CurrentTweakProfileIndex == INDEX_NONE;
            if (ImGui::Selectable("None", IsSelected))
            {
                Replicator->Server_SetTweakProfile(INDEX_NONE);
            }
        }

        for (int32 TweakProfileIndex = 0; TweakProfileIndex < Asset->TweakProfiles.Num(); ++TweakProfileIndex)
        {
            const FCogAbilityTweakProfile& TweakProfile = Asset->TweakProfiles[TweakProfileIndex];
            const bool IsSelected = TweakProfileIndex == CurrentTweakProfileIndex;

            if (ImGui::Selectable(TCHAR_TO_ANSI(*TweakProfile.Name.ToString()), IsSelected))
            {
                Replicator->Server_SetTweakProfile(TweakProfileIndex);
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (ImGui::BeginTable("Tweaks", 1 + Asset->TweaksCategories.Num(), ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (int32 TweakCategoryIndex = 0; TweakCategoryIndex < Asset->TweaksCategories.Num(); ++TweakCategoryIndex)
        {
            const FCogAbilityTweakCategory& Category = Asset->TweaksCategories[TweakCategoryIndex];
            ImGui::TableSetupColumn(TCHAR_TO_ANSI(*Category.Name), ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        int32 TweakIndex = 0;
        for (const FCogAbilityTweak& Tweak : Asset->Tweaks)
        {
            if (Tweak.Effect != nullptr)
            {
                ImGui::PushID(TweakIndex);

                ImGui::TableNextColumn();
                ImGui::Text("%s", TCHAR_TO_ANSI(*Tweak.Name.ToString()));

                for (int TweakCategoryIndex = 0; TweakCategoryIndex < Asset->TweaksCategories.Num(); ++TweakCategoryIndex)
                {
                    ImGui::TableNextColumn();
                    ImGui::PushID(TweakCategoryIndex);
                    DrawTweak(Replicator, TweakIndex, TweakCategoryIndex);
                    ImGui::PopID();
                }

                ImGui::PopID();
                TweakIndex++;
            }
        }

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tweaks::DrawTweak(ACogAbilityReplicator* Replicator, int32 TweakIndex, int32 TweakCategoryIndex)
{
    if (Asset->TweaksCategories.IsValidIndex(TweakCategoryIndex) == false)
    {
        return;
    }

    float* Value = Replicator->GetTweakCurrentValuePtr(TweakIndex, TweakCategoryIndex);
    if (Value == nullptr)
    {
        return;
    }

    const FCogAbilityTweakCategory& Category = Asset->TweaksCategories[TweakCategoryIndex];

    FCogWidgets::PushBackColor(FCogImguiHelper::ToImVec4(Category.Color));
    ImGui::PushItemWidth(-1);
    ImGui::SliderFloat("##Value", Value, Asset->TweakMinValue, Asset->TweakMaxValue, "%+0.0f%%", 1.0f);
    ImGui::PopItemWidth();
    FCogWidgets::PopBackColor();

    bool bUpdateValue = ImGui::IsItemDeactivatedAfterEdit();

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Button("Reset"))
        {
            *Value = 0.f;
            bUpdateValue = true;
        }
        ImGui::EndPopup();
    }

    if (bUpdateValue)
    {
        Replicator->Server_SetTweakValue(TweakIndex, TweakCategoryIndex, *Value);
    }
}