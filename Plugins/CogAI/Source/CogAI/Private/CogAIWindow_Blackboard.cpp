#include "CogAIWindow_Blackboard.h"

#include "CogWindowWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogAIWindow_Blackboard::RenderHelp()
{
    ImGui::Text(
        "This window displays the blackboard of the selected actor. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogAIWindow_Blackboard::UCogAIWindow_Blackboard()
{
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAIWindow_Blackboard::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginTable("Actions", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
    {
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Inject", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();


        ImGui::TableNextColumn();
        ImGui::TableNextColumn();

        ImGui::EndTable();
    }
}
