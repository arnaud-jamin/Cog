#include "CogEngineWindow_CollisionViewer.h"

#include "CogCommon.h"
#include "CogDebugDrawHelper.h"
#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionViewer::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogEngineConfig_CollisionViewer>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionViewer::RenderHelp()
{
    ImGui::Text("This window is used to inspect collisions by performing a collision query with the selected channels. "
        "The query can be configured in the options. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionViewer::RenderContent()
{
    Super::RenderContent();

    const APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    //-------------------------------------------------
    // Query Profile
    //-------------------------------------------------

    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile == nullptr)
    {
        return;
    }

    //-------------------------------------------------
    // Menu
    //-------------------------------------------------
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            //-------------------------------------------------
            // Query Mode
            //-------------------------------------------------
            ImGui::Combo("Query", &Config->QueryType,
                "Sphere\0"
                "Raycast Crosshair\0"
                "Raycast Cursor\0"
                "\0"
            );

            //-------------------------------------------------
            // Query Distance
            //-------------------------------------------------
            ImGui::SliderFloat("Distance", &Config->QueryDistance, 0.0f, 20000.0f, "%0.f");

            //-------------------------------------------------
            // Query Thickness
            //-------------------------------------------------
            if (Config->QueryType == 1 || Config->QueryType == 2)
            {
                ImGui::SliderFloat("Thickness", &Config->QueryThickness, 0.0f, 1000.0f, "%0.f");
            }

            //-------------------------------------------------
            // Query Use Complex Collisions
            //-------------------------------------------------
            ImGui::Checkbox("Use Complex Collisions", &Config->UseComplexCollisions);

            //-------------------------------------------------
            // Show Names
            //-------------------------------------------------
            ImGui::Checkbox("Show Actors Names", &Config->ShowActorsNames);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    //-------------------------------------------------
    // Profile
    //-------------------------------------------------
    const FCollisionResponseTemplate* SelectedProfile = CollisionProfile->GetProfileByIndex(Config->ProfileIndex);
    FName SelectedProfileName = SelectedProfile != nullptr ? SelectedProfile->Name : FName("Custom");

    if (ImGui::BeginCombo("Profile", COG_TCHAR_TO_CHAR(*SelectedProfileName.ToString()), ImGuiComboFlags_HeightLargest))
    {
        for (int i = 0; i < CollisionProfile->GetNumOfProfiles(); ++i)
        {
            const FCollisionResponseTemplate* Profile = CollisionProfile->GetProfileByIndex(i);
            if (ImGui::Selectable(COG_TCHAR_TO_CHAR(*Profile->Name.ToString()), false))
            {
                Config->ProfileIndex = i;
                Config->ObjectTypesToQuery = 0;
                SelectedProfile = CollisionProfile->GetProfileByIndex(Config->ProfileIndex);
                
                if (Profile->CollisionEnabled != ECollisionEnabled::NoCollision)
                {
                    for (int j = 0; j < ECC_MAX; ++j)
                    {
                        ECollisionResponse Response = Profile->ResponseToChannels.GetResponse(static_cast<ECollisionChannel>(j));
                        if (Response != ECR_Ignore)
                        {
                            Config->ObjectTypesToQuery |= ECC_TO_BITFIELD(j);
                        }
                    }
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();

    FCogWidgets::CollisionObjectTypeChannels(Config->ObjectTypesToQuery);

    //-------------------------------------------------
    // Perform Query
    //-------------------------------------------------
    if (Config->ObjectTypesToQuery == 0)
    {
        return;
    }

    FVector QueryStart;
    FVector QueryEnd;
    float QueryRadius = 0.0f;

    switch (Config->QueryType)
    {
        case 0:
        {
            FVector Location = FVector::ZeroVector;
            if (APawn* Pawn = PlayerController->GetPawn())
            {
                Location = Pawn->GetActorLocation();
            }

            QueryRadius = Config->QueryDistance;
            QueryStart = Location;
            QueryEnd = QueryStart;
            break;
        }

        case 1:
        {
            FVector Location;
            FRotator Rotation;
            PlayerController->GetPlayerViewPoint(Location, Rotation);
            QueryStart = Location;
            QueryEnd = QueryStart + Rotation.Vector() * Config->QueryDistance;
            QueryRadius = Config->QueryThickness;
            break;
        }

        case 2:
        {
            FVector Direction;
            UGameplayStatics::DeprojectScreenToWorld(PlayerController, FCogImguiHelper::ToFVector2D(ImGui::GetMousePos()), QueryStart, Direction);
            QueryEnd = QueryStart + Direction * Config->QueryDistance;
            QueryRadius = Config->QueryThickness;
            break;
        }

        default: break;
    }

    static const FName TraceTag(TEXT("FCogWindow_Collision"));
    FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogHitDetection), Config->UseComplexCollisions);

    FCollisionObjectQueryParams QueryObjectParams;
    QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;

    FCollisionShape QueryShape;
    QueryShape.SetSphere(QueryRadius);

    TArray<FHitResult> HitResults;
    UWorld* World = GetWorld();
    World->SweepMultiByObjectType(
        HitResults,
        QueryStart,
        QueryEnd,
        FQuat::Identity,
        QueryObjectParams,
        QueryShape,
        QueryParams);

    FCogDebugDrawSweepParams DrawParams;
    FCogDebug::GetDebugDrawSweepSettings(DrawParams);
    DrawParams.DrawHitNormals = false;
    DrawParams.DrawHitImpactNormals = false;
    DrawParams.DrawHitImpactPoints = false;
    DrawParams.DrawHitLocation = false;
    DrawParams.DrawHitPrimitives = true;
    DrawParams.DrawHitPrimitiveActorsName = Config->ShowActorsNames;
    FCogDebugDrawHelper::DrawSweep(GetWorld(), QueryShape, QueryStart, QueryEnd, FQuat::Identity, false, HitResults, DrawParams);
}
