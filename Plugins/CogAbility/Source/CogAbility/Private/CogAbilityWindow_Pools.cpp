#include "CogAbilityWindow_Pools.h"

#include "AbilitySystemComponent.h"
#include "CogImguiHelper.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogAbilityWindow_Pools::UCogAbilityWindow_Pools()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void DrawPool(const UAbilitySystemComponent* AbilitySystemComponent, const FCogAbilityPool& Pool)
{
    if (AbilitySystemComponent->HasAttributeSetForAttribute(Pool.Max) == false)
    {
        return;
    }

    if (AbilitySystemComponent->HasAttributeSetForAttribute(Pool.Value) == false)
    {
        return;
    }

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", TCHAR_TO_ANSI(*Pool.Name));

    const float Value = AbilitySystemComponent->GetNumericAttribute(Pool.Value);
    const float Max = AbilitySystemComponent->GetNumericAttribute(Pool.Max);

    //-------------------------------------------------------------------------------------------
    // Use a different format base on max value for all pools to be nicely aligned at the center
    //-------------------------------------------------------------------------------------------
    const char* format = nullptr;
    if (Max >= 100)     { format = "%3.0f / %3.0f"; }  //     |200 / 200|        |__1 / 200|        3 characters with 0 floating point
    else if (Max >= 10) { format = "%4.1f / %4.1f"; }  //    |20.0 / 20.0|      |_1.1 / 20.0|       4 characters with 1 floating point
    else                { format = "%3.2f / %3.2f"; }  //    |2.00 / 2.00|      |1.11 / 2.00|       3 characters with 2 floating points

    char Buffer[64];
    ImFormatString(Buffer, IM_ARRAYSIZE(Buffer), format, Value, Max);

    const float Ratio = Max > 0.0f ? Value / Max : 0.0f;

    ImGui::TableNextColumn();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, FCogImguiHelper::ToImVec4(Pool.Color));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, FCogImguiHelper::ToImVec4(Pool.BackColor));
    FCogWindowWidgets::ProgressBarCentered(Ratio, ImVec2(-1, 0), Buffer);
    ImGui::PopStyleColor(2);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Pools::RenderContent()
{
    Super::RenderContent();

    if (PoolsAsset == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystem == nullptr)
    {
        return;
    }

    if (ImGui::BeginTable("Pools", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Pool", ImGuiTableColumnFlags_WidthStretch);

        for (const FCogAbilityPool& Pool : PoolsAsset->Pools)
        {
            DrawPool(AbilitySystem, Pool);
        }

        ImGui::EndTable();
    }
}
