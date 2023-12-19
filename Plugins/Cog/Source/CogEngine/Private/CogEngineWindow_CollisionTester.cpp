#include "CogEngineWindow_CollisionTester.h"

#include "CogDebugDrawHelper.h"
#include "CogDebugSettings.h"
#include "CogEngineDataAsset.h"
#include "CogImGuiHelper.h"
#include "CogWindowWidgets.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    SetAsset(GetAsset<UCogEngineDataAsset>());

    Config = GetConfig<UCogEngineConfig_CollisionTester>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::RenderHelp()
{
    ImGui::Text("This window is used to inspect collisions by performing a collision query with the selected channels. "
        "The query can be configured in the options. "
        "The displayed collision channels can be configured in the '%s' data asset. "
        , TCHAR_TO_ANSI(*GetNameSafe(Asset.Get()))
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::RenderContent()
{
    Super::RenderContent();

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
            ImGui::Checkbox("Draw Hit Locations", &Config->DrawHitLocations);
            ImGui::Checkbox("Draw Hit Impact Points", &Config->DrawHitImpactPoints);
            ImGui::Checkbox("Draw Hit Shapes", &Config->DrawHitShapes);
            ImGui::Checkbox("Draw Hit Normal", &Config->DrawHitNormals);
            ImGui::Checkbox("Draw Hit Impact Normal", &Config->DrawHitImpactNormals);
            ImGui::Checkbox("Draw Hit Primitives", &Config->DrawHitPrimitives);
            ImGui::Checkbox("Draw Hit Actors Names", &Config->DrawHitActorsNames);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Hit Point Size", &Config->HitPointSize, 0.0f, 20.0f, "%0.f");

            ImGui::ColorEdit4("No Hit Color", (float*)&Config->NoHitColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Hit Color", (float*)&Config->HitColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Normal Color", (float*)&Config->NormalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Impact Normal Color", (float*)&Config->ImpactNormalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("Placement", Config->Placement);

    if (Config->Placement == ECogEngine_CollisionQueryPlacement::Selection)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat3("Location", &Config->LocationStart.X, 1.0f, 0.0f, 0.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat3("Rotation", &Config->Rotation.Pitch, 1.0f, 0.0f, 0.0f, "%.1f");
    }
    else if (Config->Placement == ECogEngine_CollisionQueryPlacement::Transform)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat3("Start Location", &Config->LocationStart.X, 1.0f, 0.0f, 0.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat3("End Location", &Config->LocationEnd.X, 1.0f, 0.0f, 0.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat3("Rotation", &Config->Rotation.Pitch, 1.0f, 0.0f, 0.0f, "%.1f");
    }

    if (Config->Type == ECogEngine_CollisionQueryType::LineTrace || Config->Type == ECogEngine_CollisionQueryType::Sweep)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::SliderFloat("Distance", &Config->QueryLength, 0.0f, 20000.0f, "%0.f");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("Type", Config->Type);

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("By", Config->By);

    //-------------------------------------------------
    // Channel
    //-------------------------------------------------
    if (Config->By == ECogEngine_CollisionQueryBy::Channel)
    {
        const FName SelectedChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(Config->Channel);

        FCogWindowWidgets::SetNextItemToShortWidth();
        if (ImGui::BeginCombo("Channel", TCHAR_TO_ANSI(*SelectedChannelName.ToString()), ImGuiComboFlags_HeightLarge))
        {
            for (int32 ChannelIndex = 0; ChannelIndex < (int32)ECC_MAX; ++ChannelIndex)
            {
                const FChannel& Channel = Channels[ChannelIndex];
                if (Channel.IsValid == false)
                {
                    continue;
                }

                ImGui::PushID(ChannelIndex);

                const FName ChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(ChannelIndex);

                if (ImGui::Selectable(TCHAR_TO_ANSI(*ChannelName.ToString())))
                {
                    Config->Channel = ChannelIndex;
                }

                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    }
    //-------------------------------------------------
    // Profile
    //-------------------------------------------------
    else if (Config->By == ECogEngine_CollisionQueryBy::Profile)
    {
        const FCollisionResponseTemplate* SelectedProfile = CollisionProfile->GetProfileByIndex(Config->ProfileIndex);
        FName SelectedProfileName = SelectedProfile != nullptr ? SelectedProfile->Name : FName("Custom");

        FCogWindowWidgets::SetNextItemToShortWidth();
        if (ImGui::BeginCombo("Profile", TCHAR_TO_ANSI(*SelectedProfileName.ToString()), ImGuiComboFlags_HeightLargest))
        {
            for (int i = 0; i < CollisionProfile->GetNumOfProfiles(); ++i)
            {
                const FCollisionResponseTemplate* Profile = CollisionProfile->GetProfileByIndex(i);
                if (ImGui::Selectable(TCHAR_TO_ANSI(*Profile->Name.ToString()), false))
                {
                    Config->ProfileIndex = i;
                    Config->ObjectTypesToQuery = 0;
                    SelectedProfile = CollisionProfile->GetProfileByIndex(Config->ProfileIndex);

                    if (Profile->CollisionEnabled != ECollisionEnabled::NoCollision)
                    {
                        for (int j = 0; j < ECC_MAX; ++j)
                        {
                            ECollisionResponse Response = Profile->ResponseToChannels.GetResponse((ECollisionChannel)j);
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

        //-------------------------------------------------
        // Query Filtering
        //-------------------------------------------------
        for (int ChannelIndex = 0; ChannelIndex < (int32)ECC_MAX; ++ChannelIndex)
        {
            const FChannel& Channel = Channels[ChannelIndex];
            if (Channel.IsValid == false)
            {
                continue;
            }

            ImGui::PushID(ChannelIndex);

            ImColor Color = FCogImguiHelper::ToImColor(Channel.Color);
            ImGui::ColorEdit4("Color", (float*)&Color.Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            bool IsCollisionActive = (Config->ObjectTypesToQuery & ECC_TO_BITFIELD(ChannelIndex)) > 0;
            const FName ChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(ChannelIndex);
            if (ImGui::Checkbox(TCHAR_TO_ANSI(*ChannelName.ToString()), &IsCollisionActive))
            {
                if (IsCollisionActive)
                {
                    Config->ObjectTypesToQuery |= ECC_TO_BITFIELD(ChannelIndex);
                    Config->ProfileIndex = INDEX_NONE;
                }
                else
                {
                    Config->ObjectTypesToQuery &= ~ECC_TO_BITFIELD(ChannelIndex);
                    Config->ProfileIndex = INDEX_NONE;
                }
            }

            ImGui::PopID();
        }
    }

    //-------------------------------------------------
    // Shape
    //-------------------------------------------------
    if (Config->Type != ECogEngine_CollisionQueryType::LineTrace)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        FCogWindowWidgets::ComboboxEnum("Shape", Config->Shape);

        switch (Config->Shape)
        {
            case ECogEngine_CollisionQueryShape::Sphere:
            {
                FCogWindowWidgets::SetNextItemToShortWidth();
                ImGui::DragFloat("Sphere Radius", &Config->ShapeExtent.X, 0.1f, 0, 100.0f, "%.1f");
                break;
            }

            case ECogEngine_CollisionQueryShape::Box:
            {
                FCogWindowWidgets::SetNextItemToShortWidth();
                ImGui::DragFloat3("Box Extent", &Config->ShapeExtent.X, 0.1f, 0, 100.0f, "%.1f");
                break;
            }

            case ECogEngine_CollisionQueryShape::Capsule:
            {
                FCogWindowWidgets::SetNextItemToShortWidth();
                ImGui::DragFloat("Capsule Radius", &Config->ShapeExtent.X, 0.1f, 0, 100.0f, "%.1f");

                FCogWindowWidgets::SetNextItemToShortWidth();
                ImGui::DragFloat("Capsule Half Height", &Config->ShapeExtent.Z, 0.1f, 0, 100.0f, "%.1f");
                break;
            }
        }

    }

    ImGui::Checkbox("Multi", &Config->MultiHits);
    ImGui::Checkbox("Complex", &Config->TraceComplex);

    Query();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::Query()
{
    AlreadyDrawnActors.Empty();
    AlreadyDrawnComponents.Empty();

    const APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    FVector QueryStart = FVector::ZeroVector;
    FVector QueryEnd = FVector::ZeroVector;
    FQuat QueryRotation = FQuat::Identity;
    TArray<FHitResult> Hits;
    TArray<FOverlapResult> Overlaps;
    bool HasHits = false;

    switch (Config->Placement)
    {
        case ECogEngine_CollisionQueryPlacement::Selection:
        {
            if (const AActor* Selection = GetSelection())
            {
                QueryStart = Selection->GetActorLocation() + FVector(Config->LocationStart);
                QueryRotation = Selection->GetActorQuat() * FQuat(FRotator(Config->Rotation));
                QueryEnd = QueryStart + Selection->GetActorQuat().GetForwardVector() * Config->QueryLength;
            }

            break;
        }

        case ECogEngine_CollisionQueryPlacement::View:
        {
            FRotator Rotation;
            PlayerController->GetPlayerViewPoint(QueryStart, Rotation);
            QueryRotation = FQuat(Rotation);
            QueryEnd = QueryStart + QueryRotation.GetForwardVector() * Config->QueryLength;
            break;
        }

        case ECogEngine_CollisionQueryPlacement::Cursor:
        {
            FVector Direction;
            const ImGuiViewport* Viewport = ImGui::GetMainViewport();
            const ImVec2 ViewportPos = Viewport != nullptr ? Viewport->Pos : ImVec2(0, 0);
            UGameplayStatics::DeprojectScreenToWorld(PlayerController, FCogImguiHelper::ToFVector2D(ImGui::GetMousePos() - ViewportPos), QueryStart, Direction);
            QueryEnd = QueryStart + Direction * Config->QueryLength;
            break;
        }

        case ECogEngine_CollisionQueryPlacement::Transform:
        {
            QueryStart = FVector(Config->LocationStart);
            QueryEnd = FVector(Config->LocationEnd);
            QueryRotation = FQuat(FRotator(Config->Rotation));
            break;
        }
    }

    static const FName TraceTag(TEXT("FCogWindow_Collision"));
    FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogHitDetection), Config->TraceComplex);

    FCollisionShape QueryShape;

    const bool bIsUsingShape = Config->Type == ECogEngine_CollisionQueryType::Overlap || Config->Type == ECogEngine_CollisionQueryType::Sweep;
    if (bIsUsingShape)
    {
        switch (Config->Shape)
        {
            case ECogEngine_CollisionQueryShape::Sphere:    QueryShape.SetSphere(Config->ShapeExtent.X); break;
            case ECogEngine_CollisionQueryShape::Capsule:   QueryShape.SetCapsule(Config->ShapeExtent.X, Config->ShapeExtent.Z); break;
            case ECogEngine_CollisionQueryShape::Box:       QueryShape.SetBox(Config->ShapeExtent); break;
        }
    }

    switch (Config->Type)
    {
    case ECogEngine_CollisionQueryType::Overlap:
    {
        switch (Config->By)
        {
            case ECogEngine_CollisionQueryBy::Channel:
            {
                HasHits = GetWorld()->OverlapMultiByChannel(Overlaps, QueryStart, QueryRotation, (ECollisionChannel)Config->Channel, QueryShape, QueryParams);
                break;
            }

            case ECogEngine_CollisionQueryBy::ObjectType:
            {
                FCollisionObjectQueryParams QueryObjectParams;
                QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;
                HasHits = GetWorld()->OverlapMultiByObjectType(Overlaps, QueryStart, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                break;
            }

            case ECogEngine_CollisionQueryBy::Profile:
            {
                HasHits = GetWorld()->OverlapMultiByProfile(Overlaps, QueryStart, QueryRotation, Config->Profile, QueryShape, QueryParams);
                break;
            }
        }

        break;
    }

    case ECogEngine_CollisionQueryType::LineTrace:
    {
        switch (Config->By)
        {
            case ECogEngine_CollisionQueryBy::Channel:
            {
                if (Config->MultiHits)
                {
                    HasHits = GetWorld()->LineTraceMultiByChannel(Hits, QueryStart, QueryEnd, (ECollisionChannel)Config->Channel, QueryParams);
                }
                else
                {
                    FHitResult Hit;
                    HasHits = GetWorld()->LineTraceSingleByChannel(Hit, QueryStart, QueryEnd, (ECollisionChannel)Config->Channel, QueryParams);
                    if (HasHits)
                    {
                        Hits.Add(Hit);
                    }
                }
                break;
            }

            case ECogEngine_CollisionQueryBy::ObjectType:
            {
                FCollisionObjectQueryParams QueryObjectParams;
                QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;

                if (Config->MultiHits)
                {
                    HasHits = GetWorld()->LineTraceMultiByObjectType(Hits, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                }
                else
                {
                    FHitResult Hit;
                    HasHits = GetWorld()->LineTraceSingleByObjectType(Hit, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                    if (HasHits)
                    {
                        Hits.Add(Hit);
                    }
                }
                break;
            }

            case ECogEngine_CollisionQueryBy::Profile:
            {
                if (Config->MultiHits)
                {
                    GetWorld()->LineTraceMultiByProfile(Hits, QueryStart, QueryEnd, Config->Profile, QueryParams);
                }
                else
                {
                    FHitResult Hit;
                    HasHits = GetWorld()->LineTraceSingleByProfile(Hit, QueryStart, QueryEnd, Config->Profile, QueryParams);
                    if (HasHits)
                    {
                        Hits.Add(Hit);
                    }
                }
                break;
            }
        }
        break;
    }

    case ECogEngine_CollisionQueryType::Sweep:
    {
        switch (Config->By)
        {
        case ECogEngine_CollisionQueryBy::Channel:
        {
            if (Config->MultiHits)
            {
                HasHits = GetWorld()->SweepMultiByChannel(Hits, QueryStart, QueryEnd, QueryRotation, (ECollisionChannel)Config->Channel, QueryShape, QueryParams);
            }
            else
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByChannel(Hit, QueryStart, QueryEnd, QueryRotation, (ECollisionChannel)Config->Channel, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::ObjectType:
        {
            if (Config->MultiHits)
            {
                FCollisionObjectQueryParams QueryObjectParams;
                QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;
                HasHits = GetWorld()->SweepMultiByObjectType(Hits, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
            }
            else
            {
                FCollisionObjectQueryParams QueryObjectParams;
                QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;

                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByObjectType(Hit, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::Profile:
        {
            if (Config->MultiHits)
            {
                HasHits = GetWorld()->SweepMultiByProfile(Hits, QueryStart, QueryEnd, QueryRotation, Config->Profile, QueryShape, QueryParams);
            }
            else
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByProfile(Hit, QueryStart, QueryEnd, QueryRotation, Config->Profile, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
            }
            break;
        }
        }
        break;
    }
    }

    const FColor Color = HasHits ? FLinearColor(Config->HitColor).ToFColor(true) : FLinearColor(Config->NoHitColor).ToFColor(true);

    const bool bUseTrace = Config->Type == ECogEngine_CollisionQueryType::LineTrace || Config->Type == ECogEngine_CollisionQueryType::Sweep;
    if (bUseTrace)
    {
        DrawDebugDirectionalArrow(
            GetWorld(),
            QueryStart,
            QueryEnd,
            FCogDebugSettings::Data.ArrowSize,
            Color,
            false,
            0.0f,
            FCogDebugSettings::GetDebugDepthPriority(0),
            FCogDebugSettings::GetDebugThickness(0.0f));
    }

    if (bIsUsingShape)
    {
        DrawShape(QueryShape, QueryStart, QueryRotation, FVector::OneVector, Color, false);
    }

    for (const FOverlapResult& Overlap : Overlaps)
    {
        if (Config->DrawHitPrimitives)
        {
            DrawPrimitive(Overlap.GetComponent());
        }
    }

    for (const FHitResult& Hit : Hits)
    {
        if (Config->DrawHitLocations)
        {
            DrawDebugPoint(
                GetWorld(),
                Hit.Location,
                Config->HitPointSize,
                Color,
                false,
                0.0f,
                FCogDebugSettings::GetDebugDepthPriority(0));
        }

        if (Config->DrawHitImpactPoints)
        {
            DrawDebugPoint(
                GetWorld(),
                Hit.ImpactPoint,
                Config->HitPointSize,
                Color,
                false,
                0.0f,
                FCogDebugSettings::GetDebugDepthPriority(0));
        }

        if (bIsUsingShape && Config->DrawHitShapes)
        {
            DrawShape(QueryShape, Hit.Location, QueryRotation, FVector::OneVector, Color, false);
        }

        if (Config->DrawHitNormals)
        {
            DrawDebugDirectionalArrow(
                GetWorld(),
                Hit.Location,
                Hit.Location + Hit.Normal * 20.0f,
                FCogDebugSettings::Data.ArrowSize,
                FLinearColor(Config->NormalColor).ToFColor(true),
                false,
                0.0f,
                FCogDebugSettings::GetDebugDepthPriority(0),
                FCogDebugSettings::GetDebugThickness(0.0f));
        }

        if (Config->DrawHitImpactNormals)
        {
            DrawDebugDirectionalArrow(
                GetWorld(),
                Hit.ImpactPoint,
                Hit.ImpactPoint + Hit.ImpactNormal * 20.0f,
                FCogDebugSettings::Data.ArrowSize,
                FLinearColor(Config->ImpactNormalColor).ToFColor(true),
                false,
                0.0f,
                FCogDebugSettings::GetDebugDepthPriority(0),
                FCogDebugSettings::GetDebugThickness(0.0f));
        }

        if (Config->DrawHitPrimitives)
        {
            DrawPrimitive(Hit.GetComponent());
        }
    }

    if (const APlayerController* LocalPlayerController = GetLocalPlayerController())
    {
        FVector ScaleStart(Config->ShapeExtent);
        FTransform TransformStart(QueryRotation, QueryStart, ScaleStart);
        GizmoStart.Draw(*LocalPlayerController, TransformStart);
        Config->LocationStart = FVector3f(TransformStart.GetLocation());
        Config->ShapeExtent = FVector3f(TransformStart.GetScale3D());

        FVector ScaleEnd(Config->ShapeExtent);
        FTransform TransformEnd(QueryRotation, QueryEnd, ScaleEnd);
        GizmoEnd.Draw(*LocalPlayerController, TransformEnd);
        Config->LocationEnd = FVector3f(TransformEnd.GetLocation());
        Config->ShapeExtent = FVector3f(TransformEnd.GetScale3D());
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::DrawPrimitive(const UPrimitiveComponent* PrimitiveComponent)
{
    //-------------------------------------------------------
    // Don't draw same primitives multiple times (for bones)
    //-------------------------------------------------------
    if (AlreadyDrawnComponents.Contains(PrimitiveComponent))
    {
        return;
    }

    AlreadyDrawnComponents.Add(PrimitiveComponent);

    ECollisionChannel CollisionObjectType = PrimitiveComponent->GetCollisionObjectType();
    FColor Color = Channels[CollisionObjectType].Color;

    //-------------------------------------------------------
    // Draw Name
    //-------------------------------------------------------
    if (Config->DrawHitActorsNames)
    {
        const AActor* Actor = PrimitiveComponent->GetOwner();
        if (Actor != nullptr)
        {
            if (AlreadyDrawnActors.Contains(Actor) == false)
            {
                FColor TextColor = Color.WithAlpha(255);
                DrawDebugString(GetWorld(), Actor->GetActorLocation(), GetNameSafe(Actor->GetClass()), nullptr, FColor::White, 0.0f, FCogDebugSettings::Data.TextShadow, FCogDebugSettings::Data.TextSize);
                AlreadyDrawnActors.Add(Actor);
            }
        }
    }

    const FVector Location = PrimitiveComponent->GetComponentLocation();
    const FQuat Rotation = PrimitiveComponent->GetComponentQuat();
    const FVector Scale = PrimitiveComponent->GetComponentScale();
    const FCollisionShape Shape = PrimitiveComponent->GetCollisionShape();
    DrawShape(Shape, Location, Rotation, Scale, Color, true);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::DrawShape(const FCollisionShape& Shape, const FVector& Location, const FQuat& Rotation, const FVector& Scale, const FColor& Color, bool DrawSolid) const
{
    switch (Shape.ShapeType)
    {
    case ECollisionShape::Box:
    {
        //--------------------------------------------------
        // see UBoxComponent::GetScaledBoxExtent()
        //--------------------------------------------------
        const FVector HalfExtent(Shape.Box.HalfExtentX * Scale.X, Shape.Box.HalfExtentY * Scale.Y, Shape.Box.HalfExtentZ * Scale.Z);

        if (DrawSolid)
        {
            DrawDebugSolidBox(
                GetWorld(),
                Location,
                HalfExtent,
                Rotation,
                Color,
                false,
                0.0f,
                FCogDebugSettings::GetDebugDepthPriority(0));
        }

        DrawDebugBox(
            GetWorld(),
            Location,
            HalfExtent,
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
        //--------------------------------------------------
        // see USphereComponent::GetScaledSphereRadius()
        //--------------------------------------------------
        const float RadiusScale = FMath::Min(Scale.X, FMath::Min(Scale.Y, Scale.Z));
        const float Radius = Shape.Sphere.Radius * RadiusScale;

        FCogDebugDrawHelper::DrawSphere(
            GetWorld(),
            Location,
            Radius,
            FCogDebugSettings::GetCircleSegments(),
            Color,
            false,
            0.0f,
            FCogDebugSettings::GetDebugDepthPriority(0),
            FCogDebugSettings::GetDebugThickness(0.0f));
        break;
    }

    case ECollisionShape::Capsule:
    {
        //--------------------------------------------------
        // see UCapsuleComponent::GetScaledCapsuleRadius()
        //--------------------------------------------------
        const float Radius = Shape.Capsule.Radius * FMath::Min(Scale.X, Scale.Y);
        const float HalfHeight = Shape.Capsule.HalfHeight * UE_REAL_TO_FLOAT(Scale.Z);

        DrawDebugCapsule(
            GetWorld(),
            Location,
            HalfHeight,
            Radius,
            Rotation,
            Color,
            false,
            0.0f,
            FCogDebugSettings::GetDebugDepthPriority(0),
            FCogDebugSettings::GetDebugThickness(0.0f));
        break;
    }
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::SetAsset(const UCogEngineDataAsset* Value)
{
    Asset = Value;

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
