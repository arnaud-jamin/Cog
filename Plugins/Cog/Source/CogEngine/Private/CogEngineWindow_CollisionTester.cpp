#include "CogEngineWindow_CollisionTester.h"

#include "CogDebugDrawHelper.h"
#include "CogDebug.h"
#include "CogEngineDataAsset.h"
#include "CogImGuiHelper.h"
#include "CogWindowWidgets.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "imgui.h"

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
    ImGui::Text("This window is used to test a collision query.");
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
    FCogWindowWidgets::ComboboxEnum("Type", Config->Type);

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("By", Config->By);

    //-------------------------------------------------
    // Channel
    //-------------------------------------------------
    if (Config->By == ECogEngine_CollisionQueryBy::Channel)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ECollisionChannel Channel = Config->Channel.GetValue();
        if (FCogWindowWidgets::ComboCollisionChannel("Channel", Channel))
        {
            Config->Channel = Channel;
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

    ImGui::Checkbox("Multi", &Config->MultiHits);
    ImGui::Checkbox("Complex", &Config->TraceComplex);

    //-------------------------------------------------
    // Shape
    //-------------------------------------------------
    ImGui::Separator();

    if (Config->Type != ECogEngine_CollisionQueryType::LineTrace)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        FCogWindowWidgets::ComboboxEnum("Shape", Config->Shape);

        switch (Config->Shape)
        {
            case ECogEngine_CollisionQueryShape::Sphere:
            {
                FCogWindowWidgets::SetNextItemToShortWidth();
                FCogImguiHelper::DragDouble("Sphere Radius", &Config->ShapeExtent.X, 0.1f, 0, FLT_MAX, "%.1f");
                break;
            }

            case ECogEngine_CollisionQueryShape::Box:
            {
                FCogWindowWidgets::SetNextItemToShortWidth();
                FCogImguiHelper::DragFVector("Box Extent", Config->ShapeExtent, 0.1f, 0, FLT_MAX, "%.1f");
                break;
            }

            case ECogEngine_CollisionQueryShape::Capsule:
            {
                FCogWindowWidgets::SetNextItemToShortWidth();
                FCogImguiHelper::DragDouble("Capsule Radius", &Config->ShapeExtent.X, 0.1f, 0, FLT_MAX, "%.1f");

                FCogWindowWidgets::SetNextItemToShortWidth();
                FCogImguiHelper::DragDouble("Capsule Half Height", &Config->ShapeExtent.Z, 0.1f, 0, FLT_MAX, "%.1f");
                break;
            }
        }
    }

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

    FVector QueryStart = Config->LocationStart;
    FVector QueryEnd = Config->LocationEnd;
    FQuat QueryRotation = FQuat(Config->Rotation);
    TArray<FHitResult> Hits;
    TArray<FOverlapResult> Overlaps;
    bool HasHits = false;
    

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
            case ECogEngine_CollisionQueryShape::Box:       QueryShape.SetBox(FVector3f(Config->ShapeExtent)); break;
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
            FCogDebug::Settings.ArrowSize,
            Color,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));
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
                FCogDebug::GetDebugDepthPriority(0));
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
                FCogDebug::GetDebugDepthPriority(0));
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
                FCogDebug::Settings.ArrowSize,
                FLinearColor(Config->NormalColor).ToFColor(true),
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0),
                FCogDebug::GetDebugThickness(0.0f));
        }

        if (Config->DrawHitImpactNormals)
        {
            DrawDebugDirectionalArrow(
                GetWorld(),
                Hit.ImpactPoint,
                Hit.ImpactPoint + Hit.ImpactNormal * 20.0f,
                FCogDebug::Settings.ArrowSize,
                FLinearColor(Config->ImpactNormalColor).ToFColor(true),
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0),
                FCogDebug::GetDebugThickness(0.0f));
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
        GizmoStart.Draw("Query Start", *LocalPlayerController, TransformStart);
        Config->LocationStart = TransformStart.GetLocation();
        Config->Rotation = FRotator(TransformStart.GetRotation());
        Config->ShapeExtent = TransformStart.GetScale3D();

        if (Config->Type != ECogEngine_CollisionQueryType::Overlap)
        {
            FTransform TransformEnd(FRotator::ZeroRotator, QueryEnd, FVector::OneVector);
            GizmoEnd.Draw("Query End", *LocalPlayerController, TransformEnd, ECogDebug_GizmoFlags::NoRotation | ECogDebug_GizmoFlags::NoScale);
            Config->LocationEnd = TransformEnd.GetLocation();
        }
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
                DrawDebugString(GetWorld(), Actor->GetActorLocation(), GetNameSafe(Actor->GetClass()), nullptr, FColor::White, 0.0f, FCogDebug::Settings.TextShadow, FCogDebug::Settings.TextSize);
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
                FCogDebug::GetDebugDepthPriority(0));
        }

        DrawDebugBox(
            GetWorld(),
            Location,
            HalfExtent,
            Rotation,
            Color,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));

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
            FCogDebug::GetCircleSegments(),
            Color,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));
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
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));
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
