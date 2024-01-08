#include "CogEngineHelper.h"

#include "CogEngineReplicator.h"
#include "CogWindowHelper.h"
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
    const UObject* DefaultObject = Actor.GetClass()->GetDefaultObject();
    if (DefaultObject == nullptr)
    {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Browse To Asset", ImVec2(-1, 0)))
    {
		IAssetTools::Get().SyncBrowserToAssets({ DefaultObject });
    }
    if (DefaultObject == nullptr)
    {
        ImGui::EndDisabled();
    }
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