#include "CogEngineWindow_Collisions.h"

#include "CogDebugDrawHelper.h"
#include "CogDebugSettings.h"
#include "CogEngineDataAsset.h"
#include "CogImGuiHelper.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Collisions::RenderHelp()
{
    ImGui::Text("This window is used to inspect collisions by performing a collision query with the selected channels. "
        "The query can be configured in the options. "
        "The displayed collision channels can be configured in the '%s' data asset. "
        , TCHAR_TO_ANSI(*GetNameSafe(CollisionsAsset.Get()))
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Collisions::UCogEngineWindow_Collisions()
{
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------

void UCogEngineWindow_Collisions::RenderContent()
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
            ImGui::Combo("Query", &QueryType,
                "Sphere\0"
                "Raycast Crosshair\0"
                "Raycast Cursor\0"
                "\0"
            );

            //-------------------------------------------------
            // Query Distance
            //-------------------------------------------------
            ImGui::SliderFloat("Distance", &QueryDistance, 0.0f, 20000.0f, "%0.f");

            //-------------------------------------------------
            // Query Thickness
            //-------------------------------------------------
            if (QueryType == 1 || QueryType == 2)
            {
                ImGui::SliderFloat("Thickness", &QueryThickness, 0.0f, 1000.0f, "%0.f");
            }

            //-------------------------------------------------
            // Query Use Complex Collisions
            //-------------------------------------------------
            ImGui::Checkbox("Use Complex Collisions", &UseComplexCollisions);

            //-------------------------------------------------
            // Show Names
            //-------------------------------------------------
            ImGui::Checkbox("Show Actors Names", &ShowActorsNames);

            //-------------------------------------------------
            // Show Query
            //-------------------------------------------------
            ImGui::Checkbox("Show Query", &ShowQuery);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    //-------------------------------------------------
    // Profile
    //-------------------------------------------------
    const FCollisionResponseTemplate* SelectedProfile = CollisionProfile->GetProfileByIndex(ProfileIndex);
    FName SelectedProfileName = SelectedProfile != nullptr ? SelectedProfile->Name : FName("Custom");

    if (ImGui::BeginCombo("Profile", TCHAR_TO_ANSI(*SelectedProfileName.ToString()), ImGuiComboFlags_HeightLargest))
    {
        for (int i = 0; i < CollisionProfile->GetNumOfProfiles(); ++i)
        {
            const FCollisionResponseTemplate* Profile = CollisionProfile->GetProfileByIndex(i);
            if (ImGui::Selectable(TCHAR_TO_ANSI(*Profile->Name.ToString()), false))
            {
                ProfileIndex = i;
                ObjectTypesToQuery = 0;
                SelectedProfile = CollisionProfile->GetProfileByIndex(ProfileIndex);
                
                if (Profile->CollisionEnabled != ECollisionEnabled::NoCollision)
                {
                    for (int j = 0; j < ECC_MAX; ++j)
                    {
                        ECollisionResponse Response = Profile->ResponseToChannels.GetResponse((ECollisionChannel)j);
                        if (Response != ECR_Ignore)
                        {
                            ObjectTypesToQuery |= ECC_TO_BITFIELD(j);
                        }
                    }
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();

    //-------------------------------------------------
    // Query Filtering
    //-------------------------------------------------
    for (int ChannelIndex = 0; ChannelIndex < (int32)ECC_MAX; ++ChannelIndex)
    {
        ImGui::PushID(ChannelIndex);

        const FChannel& Channel = Channels[ChannelIndex];
        if (Channel.IsValid == false)
        {
            continue;
        }

        ImColor Color = FCogImguiHelper::ToImColor(Channel.Color);
        ImGui::ColorEdit4("Color", (float*)&Color.Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();

        bool IsCollisionActive = (ObjectTypesToQuery & ECC_TO_BITFIELD(ChannelIndex)) > 0;
        const FName ChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(ChannelIndex);
        if (ImGui::Checkbox(TCHAR_TO_ANSI(*ChannelName.ToString()), &IsCollisionActive))
        {
            if (IsCollisionActive)
            {
                ObjectTypesToQuery |= ECC_TO_BITFIELD(ChannelIndex);
                ProfileIndex = INDEX_NONE;
            }
            else
            {
                ObjectTypesToQuery &= ~ECC_TO_BITFIELD(ChannelIndex);
                ProfileIndex = INDEX_NONE;
            }
        }

        ImGui::PopID();
    }

    //-------------------------------------------------
    // Perform Query
    //-------------------------------------------------
    if (ObjectTypesToQuery == 0)
    {
        return;
    }

    FVector QueryStart;
    FVector QueryEnd;
    float QueryRadius = 0.0f;

    switch (QueryType)
    {
        case 0:
        {
            FVector Location = FVector::ZeroVector;
            if (APawn* Pawn = PlayerController->GetPawn())
            {
                Location = Pawn->GetActorLocation();
            }

            QueryRadius = QueryDistance;
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
            QueryEnd = QueryStart + Rotation.Vector() * QueryDistance;
            QueryRadius = QueryThickness;
            break;
        }

        case 2:
        {
            FVector Direction;
            UGameplayStatics::DeprojectScreenToWorld(PlayerController, FCogImguiHelper::ToVector2D(ImGui::GetMousePos()), QueryStart, Direction);
            QueryEnd = QueryStart + Direction * QueryDistance;
            QueryRadius = QueryThickness;
            break;
        }
    }

    static const FName TraceTag(TEXT("FCogWindow_Collision"));
    FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogHitDetection), UseComplexCollisions);

    FCollisionObjectQueryParams QueryObjectParams;
    QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;

    FCollisionShape QueryShape;
    QueryShape.SetSphere(QueryRadius);

    TArray<FHitResult> QueryHits;
    UWorld* World = GetWorld();
    World->SweepMultiByObjectType(
        QueryHits,
        QueryStart,
        QueryEnd,
        FQuat::Identity,
        QueryObjectParams,
        QueryShape,
        QueryParams);

    if (ShowQuery)
    {
        FCogDebugDrawHelper::DrawCapsuleCastMulti(World, QueryStart, QueryEnd, FQuat::Identity, 0.0f, QueryRadius, EDrawDebugTrace::ForOneFrame, false, QueryHits, FLinearColor::White, FLinearColor::Red, FCogDebugSettings::GetDebugDuration(true));
    }

    TSet<const AActor*> AlreadyDrawnActors;
    TSet<const UPrimitiveComponent*> AlreadyDrawnComponents;

    for (const FHitResult& HitResult : QueryHits)
    {
        //-------------------------------------------------------
        // Don't draw same primitives multiple times (for bones)
        //-------------------------------------------------------
        const UPrimitiveComponent* PrimitiveComponent = HitResult.GetComponent();
        if (AlreadyDrawnComponents.Contains(PrimitiveComponent))
        {
            continue;
        }
        AlreadyDrawnComponents.Add(PrimitiveComponent);

        ECollisionChannel CollisionObjectType = PrimitiveComponent->GetCollisionObjectType();
        FColor Color = Channels[CollisionObjectType].Color;

        //-------------------------------------------------------
        // Draw Name
        //-------------------------------------------------------
        if (ShowActorsNames)
        {
            const AActor* Actor = HitResult.GetActor();
            if (Actor != nullptr)
            {
                if (AlreadyDrawnActors.Contains(Actor) == false)
                {
                    FColor TextColor = Color.WithAlpha(255);
                    DrawDebugString(World, Actor->GetActorLocation(), GetNameSafe(Actor->GetClass()), nullptr, FColor::White, 0.0f, FCogDebugSettings::TextShadow, FCogDebugSettings::TextSize);
                    AlreadyDrawnActors.Add(Actor);
                }
            }
        }

        //-------------------------------------------------------
        // Draw Shape
        //-------------------------------------------------------
        FCollisionShape Shape = PrimitiveComponent->GetCollisionShape();
        switch (Shape.ShapeType)
        {
            case ECollisionShape::Box:
            {
                FVector Location;
                FVector Extent;
                FQuat Rotation;
                    
                if (const UBoxComponent* BoxComponent = Cast<UBoxComponent>(PrimitiveComponent))
                {
                    Location = BoxComponent->GetComponentLocation();
                    Extent = BoxComponent->GetScaledBoxExtent();
                    Rotation = BoxComponent->GetComponentQuat();
                }
                else
                {
                    PrimitiveComponent->Bounds.GetBox().GetCenterAndExtents(Location, Extent);
                    Extent += FVector::OneVector;
                    Rotation = FQuat::Identity;
                }

                DrawDebugSolidBox(
                    World,
                    Location,
                    Extent,
                    Rotation,
                    Color,
                    false,
                    0.0f,
                    FCogDebugSettings::GetDebugDepthPriority(0));

                DrawDebugBox(
                    World,
                    Location,
                    Extent,
                    Rotation,
                    Color,
                    false,
                    0.0f,
                    FCogDebugSettings::GetDebugDepthPriority(0),
                    FCogDebugSettings::GetDebugThickness(0.0f));

                break;
            }

            case ECollisionShape::Sphere:
            {
                if (const USphereComponent* SphereComponent = Cast<USphereComponent>(PrimitiveComponent))
                {
                    FCogDebugDrawHelper::DrawSphere(
                        World,
                        SphereComponent->GetComponentLocation(),
                        SphereComponent->GetScaledSphereRadius(),
                        FCogDebugSettings::GetCircleSegments(),
                        Color,
                        false, 
                        0.0f, 
                        FCogDebugSettings::GetDebugDepthPriority(0),
                        FCogDebugSettings::GetDebugThickness(0.0f));
                }
                break;
            }

            case ECollisionShape::Capsule:
            {
                if (const UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(PrimitiveComponent))
                {
                    DrawDebugCapsule(World, 
                    CapsuleComponent->GetComponentLocation(), 
                    CapsuleComponent->GetScaledCapsuleHalfHeight(), 
                    CapsuleComponent->GetScaledCapsuleRadius(), 
                    CapsuleComponent->GetComponentQuat(),
                    Color, 
                    false, 
                    0.0f, 
                    FCogDebugSettings::GetDebugDepthPriority(0),
                    FCogDebugSettings::GetDebugThickness(0.0f));
                }
                break;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Collisions::SetAsset(const UCogEngineDataAsset* Asset)
{
    if (Asset == nullptr)
    {
        return;
    }

    for (FChannel& Channel : Channels)
    {
        Channel.IsValid = false;
    }

    for (const FCogCollisionChannel& AssetChannel : Asset->Channels)
    {
        FChannel& Channel = Channels[(uint8)AssetChannel.Channel];
        Channel.IsValid = true;
        Channel.Color = AssetChannel.Color.ToFColor(true);
    }
}