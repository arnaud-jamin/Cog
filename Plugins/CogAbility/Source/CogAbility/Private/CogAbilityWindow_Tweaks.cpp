#include "CogAbilityWindow_Tweaks.h"

#include "AbilitySystemComponent.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogAbilityWindow_Tweaks::UCogAbilityWindow_Tweaks()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void DrawTweak(ACogAbilityReplicator* Replicator, const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakIndex, int32 TweakCategoryIndex)
{
    if (TweaksAsset->TweaksCategories.IsValidIndex(TweakCategoryIndex) == false)
    {
        return;
    }

    float* Value = Replicator->GetTweakCurrentValuePtr(TweaksAsset, TweakIndex, TweakCategoryIndex);
    if (Value == nullptr)
    {
        return;
    }

    const FCogAbilityTweakCategory& Category = TweaksAsset->TweaksCategories[TweakCategoryIndex];

    FCogWindowWidgets::PushBackColor(FCogImguiHelper::ToImVec4(Category.Color));
    ImGui::PushItemWidth(-1);
    ImGui::SliderFloat("##Value", Value, TweaksAsset->TweakMinValue, TweaksAsset->TweakMaxValue, "%+0.0f%%", 1.0f);
    ImGui::PopItemWidth();
    FCogWindowWidgets::PopBackColor();

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
        Replicator->SetTweakValue(TweaksAsset, TweakIndex, TweakCategoryIndex, *Value);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Tweaks::RenderContent()
{
    Super::RenderContent();

    FCogAbilityModule& Module = FCogAbilityModule::Get();
    ACogAbilityReplicator* Replicator = Module.GetLocalReplicator();
    if (Replicator == nullptr)
    {
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Reset"))
        {
            Replicator->ResetAllTweaks();
        }

        ImGui::EndMenuBar();
    }

    int32 CurrentTweakProfileIndex = Replicator->GetTweakProfileIndex();
    FName CurrentProfileName = FName("None");
    if (TweaksAsset->TweakProfiles.IsValidIndex(CurrentTweakProfileIndex))
    {
        CurrentProfileName = TweaksAsset->TweakProfiles[CurrentTweakProfileIndex].Name;
    }

    if (ImGui::BeginCombo("Profile", TCHAR_TO_ANSI(*CurrentProfileName.ToString())))
    {
        {
            bool IsSelected = CurrentTweakProfileIndex == INDEX_NONE;
            if (ImGui::Selectable("None", IsSelected))
            {
                Replicator->SetTweakProfile(TweaksAsset.Get(), INDEX_NONE);
            }
        }

        for (int32 TweakProfileIndex = 0; TweakProfileIndex < TweaksAsset->TweakProfiles.Num(); ++TweakProfileIndex)
        {
            const FCogAbilityTweakProfile& TweakProfile = TweaksAsset->TweakProfiles[TweakProfileIndex];
            bool IsSelected = TweakProfileIndex == CurrentTweakProfileIndex;

            if (ImGui::Selectable(TCHAR_TO_ANSI(*TweakProfile.Name.ToString()), IsSelected))
            {
                Replicator->SetTweakProfile(TweaksAsset.Get(), TweakProfileIndex);
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (ImGui::BeginTable("Tweaks", 1 + TweaksAsset->TweaksCategories.Num(), ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (int32 TweakCategoryIndex = 0; TweakCategoryIndex < TweaksAsset->TweaksCategories.Num(); ++TweakCategoryIndex)
        {
            FCogAbilityTweakCategory& Category = TweaksAsset->TweaksCategories[TweakCategoryIndex];
            ImGui::TableSetupColumn(TCHAR_TO_ANSI(*Category.Name), ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        int32 TweakIndex = 0;
        for (FCogAbilityTweak& Tweak : TweaksAsset->Tweaks)
        {
            if (Tweak.Effect != nullptr)
            {
                ImGui::PushID(TweakIndex);

                ImGui::TableNextColumn();
                ImGui::Text("%s", TCHAR_TO_ANSI(*Tweak.Name.ToString()));

                for (int TweakCategoryIndex = 0; TweakCategoryIndex < TweaksAsset->TweaksCategories.Num(); ++TweakCategoryIndex)
                {
                    ImGui::TableNextColumn();
                    ImGui::PushID(TweakCategoryIndex);
                    DrawTweak(Replicator, TweaksAsset.Get(), TweakIndex, TweakCategoryIndex);
                    ImGui::PopID();
                }

                ImGui::PopID();
                TweakIndex++;
            }
        }

        ImGui::EndTable();
    }
}
