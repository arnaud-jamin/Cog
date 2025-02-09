#include "CogEngineHelper.h"

#include "CogEngineReplicator.h"
#include "CogWindowWidgets.h"
#include "imgui.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

#if WITH_EDITOR
#include "IAssetTools.h"
#endif


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineHelper::ActorContextMenu(AActor& Actor)
{
    FCogWindowWidgets::ThinSeparatorText("Object");

#if WITH_EDITOR
    FCogWindowWidgets::OpenObjectAssetButton(&Actor, ImVec2(-1, 0));
#endif

    if (ImGui::Button("Delete", ImVec2(-1, 0)))
    {
        if (ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*Actor.GetWorld()))
        {
            Replicator->Server_DeleteActor(&Actor);
        }
    }

    if (APawn* Pawn = Cast<APawn>(&Actor))
    {
        FCogWindowWidgets::ThinSeparatorText("Pawn");

        if (ImGui::Button("Possess", ImVec2(-1, 0)))
        {
            if (ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*Actor.GetWorld()))
            {
                Replicator->Server_Possess(Pawn);
            }

        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Possess this pawn.");
        }

        if (ImGui::Button("Reset Possession", ImVec2(-1, 0)))
        {
            if (ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*Actor.GetWorld()))
            {
                Replicator->Server_ResetPossession();
            }

        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineHelper::RenderConfigureMessage(const TWeakObjectPtr<const UCogEngineDataAsset> InAsset)
{
    if (InAsset == nullptr)
    {
        ImGui::Text("Create a DataAsset child of '%s' to configure. ", StringCast<ANSICHAR>(*UCogEngineDataAsset::StaticClass()->GetName()).Get());
    }
    else
    {
        ImGui::Text("Can be configured in the '%s' DataAsset. ", StringCast<ANSICHAR>(*GetNameSafe(InAsset.Get())).Get());
    }
}