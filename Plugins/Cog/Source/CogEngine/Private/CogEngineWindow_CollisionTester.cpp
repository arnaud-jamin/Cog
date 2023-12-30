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
bool RenderCollisionProfileChannels(const UCollisionProfile& CollisionProfileChannels, int32& Channels, const TArray<FCogCollisionChannel>& ChannelsConfig)
{
    bool Result = false;

    for (const FCogCollisionChannel& ChannelConfig : ChannelsConfig)
    {
	    const ECollisionChannel Channel = ChannelConfig.Channel.GetValue();
        ImGui::PushID(Channel);

        ImColor Color = FCogImguiHelper::ToImColor(ChannelConfig.Color);
        ImGui::ColorEdit4("Color", (float*)&Color.Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();

        bool IsCollisionActive = (Channels & ECC_TO_BITFIELD(Channel)) > 0;
        const FName ChannelName = CollisionProfileChannels.ReturnChannelNameFromContainerIndex(Channel);
        if (ImGui::Checkbox(TCHAR_TO_ANSI(*ChannelName.ToString()), &IsCollisionActive))
        {
            Result = true;

            if (IsCollisionActive)
            {
                Channels |= ECC_TO_BITFIELD(Channel);
            }
            else
            {
                Channels &= ~ECC_TO_BITFIELD(Channel);
            }
        }

        ImGui::PopID();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool RenderComboCollisionChannel(const char* Label, const UCollisionProfile& CollisionProfile, ECollisionChannel& SelectedChannel, const TArray<FCogCollisionChannel>& ChannelsConfig)
{
    const FName SelectedChannelName = CollisionProfile.ReturnChannelNameFromContainerIndex(SelectedChannel);

    bool Result = false;
    if (ImGui::BeginCombo(Label, TCHAR_TO_ANSI(*SelectedChannelName.ToString()), ImGuiComboFlags_HeightLarge))
    {
        for (const FCogCollisionChannel& ChannelConfig : ChannelsConfig)
        {
            const ECollisionChannel Channel = ChannelConfig.Channel.GetValue();
            ImGui::PushID(Channel);

            const FName ChannelName = CollisionProfile.ReturnChannelNameFromContainerIndex(Channel);

            ImColor Color = FCogImguiHelper::ToImColor(ChannelConfig.Color);
            ImGui::ColorEdit4("Color", (float*)&Color.Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            if (ImGui::Selectable(TCHAR_TO_ANSI(*ChannelName.ToString())))
            {
                SelectedChannel = Channel;
                Result = true;
            }

            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    return Result;
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
    FCogWindowWidgets::ComboboxEnum("Mode", Config->Mode);

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("By", Config->By);

    //-------------------------------------------------
    // Channel
    //-------------------------------------------------
    if (Config->By == ECogEngine_CollisionQueryBy::Channel)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ECollisionChannel Channel = Config->Channel.GetValue();
        if (RenderComboCollisionChannel("Channel", *CollisionProfile, Channel, Asset->Channels))
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
        const FName SelectedProfileName = SelectedProfile != nullptr ? SelectedProfile->Name : FName("Custom");

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
	                        const ECollisionResponse Response = Profile->ResponseToChannels.GetResponse((ECollisionChannel)j);
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
    }

    ImGui::Checkbox("Complex", &Config->TraceComplex);

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

    ImGui::Separator();

    //-------------------------------------------------
	// Channels
	//-------------------------------------------------
    if (Config->By == ECogEngine_CollisionQueryBy::Profile)
    {
        ImGui::BeginDisabled();
        RenderCollisionProfileChannels(*CollisionProfile, Config->ObjectTypesToQuery, Asset->Channels);
        ImGui::EndDisabled();
    }
    else if (Config->By == ECogEngine_CollisionQueryBy::ObjectType)
    {
        RenderCollisionProfileChannels(*CollisionProfile, Config->ObjectTypesToQuery, Asset->Channels);
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
    const FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogHitDetection), Config->TraceComplex);

    const FCollisionResponseTemplate* Profile = UCollisionProfile::Get()->GetProfileByIndex(Config->ProfileIndex);
    const FName ProfileName = Profile != nullptr ? Profile->Name : FName();

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
            HasHits = GetWorld()->OverlapMultiByChannel(Overlaps, QueryStart, QueryRotation, Config->Channel, QueryShape, QueryParams);
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
            HasHits = GetWorld()->OverlapMultiByProfile(Overlaps, QueryStart, QueryRotation, ProfileName, QueryShape, QueryParams);
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
            switch (Config->Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->LineTraceSingleByChannel(Hit, QueryStart, QueryEnd, Config->Channel, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->LineTraceMultiByChannel(Hits, QueryStart, QueryEnd, Config->Channel, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->LineTraceTestByChannel(QueryStart, QueryEnd, Config->Channel, QueryParams);
                break;
            }
            }

            break;
        }

        case ECogEngine_CollisionQueryBy::ObjectType:
        {
            FCollisionObjectQueryParams QueryObjectParams;
            QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;

            switch (Config->Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->LineTraceSingleByObjectType(Hit, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->LineTraceMultiByObjectType(Hits, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->LineTraceTestByObjectType(QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                break;
            }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::Profile:
        {
            switch (Config->Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->LineTraceSingleByProfile(Hit, QueryStart, QueryEnd, ProfileName, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->LineTraceMultiByProfile(Hits, QueryStart, QueryEnd, ProfileName, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->LineTraceTestByProfile(QueryStart, QueryEnd, ProfileName, QueryParams);
                break;
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
            switch (Config->Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByChannel(Hit, QueryStart, QueryEnd, QueryRotation, Config->Channel, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->SweepMultiByChannel(Hits, QueryStart, QueryEnd, QueryRotation, Config->Channel, QueryShape, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->SweepTestByChannel(QueryStart, QueryEnd, QueryRotation, Config->Channel, QueryShape, QueryParams);
                break;
            }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::ObjectType:
        {
            FCollisionObjectQueryParams QueryObjectParams;
            QueryObjectParams.ObjectTypesToQuery = Config->ObjectTypesToQuery;

            switch (Config->Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByObjectType(Hit, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->SweepMultiByObjectType(Hits, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->SweepTestByObjectType(QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                break;
            }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::Profile:
        {
            switch (Config->Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByProfile(Hit, QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->SweepMultiByProfile(Hits, QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->SweepTestByProfile(QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                break;
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
