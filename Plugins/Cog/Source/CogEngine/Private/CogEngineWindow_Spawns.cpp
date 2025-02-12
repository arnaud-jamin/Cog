#include "CogEngineWindow_Spawns.h"

#include "CogEngineDataAsset.h"
#include "CogEngineReplicator.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Spawns::RenderHelp()
{
    ImGui::Text(
        "This window can be used to spawn new actors in the world. "
        "The spawn list can be configured in the '%s' data asset. "
        , TCHAR_TO_ANSI(*GetNameSafe(Asset.Get()))
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Spawns::Initialize()
{
    Super::Initialize();

    Asset = GetAsset<UCogEngineDataAsset>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Spawns::RenderContent()
{
    Super::RenderContent();

    if (Asset == nullptr)
    {
        ImGui::TextDisabled("Invalid Asset");
        return;
    }

    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        ImGui::TextDisabled("Invalid Replicator");
        return;
    }

    if (Asset->SpawnGroups.Num() == 0)
    {
        ImGui::TextDisabled("No spawn group have been defined");
        return;
    }

    int32 GroupIndex = 0;
    for (const FCogEngineSpawnGroup& SpawnGroup : Asset->SpawnGroups)
    {
        RenderSpawnGroup(*Replicator, SpawnGroup, GroupIndex);
        GroupIndex++;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Spawns::RenderSpawnGroup(ACogEngineReplicator& Replicator, const FCogEngineSpawnGroup& SpawnGroup, int32 GroupIndex)
{
    if (FCogWidgets::DarkCollapsingHeader(TCHAR_TO_ANSI(*SpawnGroup.Name), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID(GroupIndex);

        const bool PushColor = (SpawnGroup.Color != FColor::Transparent);
        if (PushColor)
        {
            FCogWidgets::PushBackColor(FCogImguiHelper::ToImVec4(SpawnGroup.Color));
        }

        static int32 SelectedAssetIndex = -1;
        int32 AssetIndex = 0;

        for (const FCogEngineSpawnEntry& SpawnEntry : SpawnGroup.Spawns)
        {
            if (RenderSpawnAsset(Replicator, SpawnEntry, SelectedAssetIndex == GroupIndex))
            {
                SelectedAssetIndex = AssetIndex;
            }

            AssetIndex++;
        }

        if (PushColor)
        {
            FCogWidgets::PopBackColor();
        }

        ImGui::PopID();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Spawns::RenderSpawnAsset(ACogEngineReplicator& Replicator, const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected)
{
    bool IsPressed = false;

    //ImGui::PushStyleColor(ImGuiCol_Button, IsLastSelected ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    FString EntryName;
    if (SpawnEntry.Asset != nullptr)
    {
        EntryName = SpawnEntry.Asset->GetName();
    }
    else
    {
        EntryName = GetNameSafe(SpawnEntry.Class);
    }

    if (ImGui::Button(TCHAR_TO_ANSI(*EntryName), ImVec2(-1, 0)))
    {
        IsPressed = true;
        Replicator.Server_Spawn(SpawnEntry);
    }

    ImGui::PopStyleVar(1);
    //ImGui::PopStyleColor(1);

    return IsPressed;
}