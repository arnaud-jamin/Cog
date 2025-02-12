#include "CogAbilityWindow_Pools.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Pools::Initialize()
{
    Super::Initialize();

    Asset = GetAsset<UCogAbilityDataAsset>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Pools::RenderHelp()
{
    ImGui::Text("This window displays attributes of the selected actor as pools. ");
    FCogAbilityHelper::RenderConfigureMessage(Asset);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Pools::RenderContent()
{
    Super::RenderContent();

    if (Asset == nullptr)
    {
        ImGui::TextDisabled("Invalid Asset");
        return;
    }

    const UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetSelection(), true);
    if (AbilitySystem == nullptr)
    {
        ImGui::TextDisabled("No ability system found on selection");
        return;
    }

    if (Asset->Pools.Num() == 0)
    {
        ImGui::TextDisabled("No pool have been defined");
        return;
    }

    if (ImGui::BeginTable("Pools", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Pool", ImGuiTableColumnFlags_WidthStretch);

        for (const FCogAbilityPool& Pool : Asset->Pools)
        {
            DrawPool(AbilitySystem, Pool);
        }

        ImGui::EndTable();
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Pools::DrawPool(const UAbilitySystemComponent* AbilitySystemComponent, const FCogAbilityPool& Pool)
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
    const char* Format;
    if (Max >= 100)         { Format = "%3.0f / %3.0f"; }   //     |200 / 200|        |__1 / 200|        3 characters with 0 floating point
    else if (Max >= 10)     { Format = "%4.1f / %4.1f"; }   //    |20.0 / 20.0|      |_1.1 / 20.0|       4 characters with 1 floating point
    else                    { Format = "%3.2f / %3.2f"; }   //    |2.00 / 2.00|      |1.11 / 2.00|       3 characters with 2 floating points

    char Buffer[64];
    ImFormatString(Buffer, IM_ARRAYSIZE(Buffer), Format, Value, Max);

    const float Ratio = Max > 0.0f ? Value / Max : 0.0f;

    ImGui::TableNextColumn();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, FCogImguiHelper::ToImVec4(Pool.Color));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, FCogImguiHelper::ToImVec4(Pool.BackColor));
    FCogWidgets::ProgressBarCentered(Ratio, ImVec2(-1, 0), Buffer);
    ImGui::PopStyleColor(2);
}
