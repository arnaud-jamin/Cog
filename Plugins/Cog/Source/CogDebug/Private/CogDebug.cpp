#include "CogDebug.h"

#include "CogCommonDebugFilteredActorInterface.h"
#include "CogDebugDrawHelper.h"
#include "CogDebugReplicator.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

//--------------------------------------------------------------------------------------------------------------------------
TWeakObjectPtr<AActor> FCogDebug::Selection;
FCogDebugSettings FCogDebug::Settings = FCogDebugSettings();

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::Reset()
{
    Settings = FCogDebugSettings();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug::IsDebugActiveForObject(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return true;
    }

    if (World->GetNetMode() == NM_DedicatedServer)
    {
        return true;
    }

    bool Result = IsDebugActiveForObject_Internal(WorldContextObject, Selection.Get(), Settings.bIsFilteringBySelection);

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug::IsReplicatedDebugActiveForObject(const UObject* WorldContextObject, const AActor* ServerSelection, bool IsServerFilteringBySelection)
{
    return IsDebugActiveForObject_Internal(WorldContextObject, ServerSelection, IsServerFilteringBySelection);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug::IsDebugActiveForObject_Internal(const UObject* WorldContextObject, const AActor* InSelection, bool InIsFilteringBySelection)
{
    if (InIsFilteringBySelection == false)
    {
        return true;
    }

    if (WorldContextObject == nullptr)
    {
        return true;
    }

    const AActor* SelectionPtr = InSelection;
    if (SelectionPtr == nullptr)
    {
        return true;
    }

    const UObject* Outer = WorldContextObject;
    for (;;)
    {
        if (SelectionPtr == Outer)
        {
            return true;
        }

        if (Cast<ICogCommonDebugFilteredActorInterface>(Outer))
        {
            return false;
        }

        const UObject* NewOuter = Outer->GetOuter();
        if (NewOuter == Outer || NewOuter == nullptr)
        {
            return true;
        }

        Outer = NewOuter;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
AActor* FCogDebug::GetSelection()
{
    return Selection.Get();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::SetSelection(UWorld* World, AActor* Value)
{
    Selection = Value;

    if (World != nullptr && World->GetNetMode() == NM_Client)
    {
        if (ACogDebugReplicator* Replicator = ACogDebugReplicator::GetLocalReplicator(*World))
        {
            Replicator->Server_SetSelection(Value);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug::GetIsFilteringBySelection()
{
    return Settings.bIsFilteringBySelection;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::SetIsFilteringBySelection(UWorld* World, bool Value)
{
    Settings.bIsFilteringBySelection = Value;

    if (World != nullptr && World->GetNetMode() == NM_Client)
    {
        if (ACogDebugReplicator* Replicator = ACogDebugReplicator::GetLocalReplicator(*World))
        {
            Replicator->Server_SetIsFilteringBySelection(Value);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug::GetDebugPersistent(bool bPersistent)
{
    return Settings.Persistent && bPersistent;
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebug::GetDebugDuration(bool bPersistent)
{
    return bPersistent == false ? 0.0f : Settings.Duration;
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebug::GetDebugTextDuration(bool bPersistent)
{
    if (bPersistent)
    {
        return Settings.Persistent ? 100 : Settings.Duration;
    }
    else
    {
        return 0.0f;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogDebug::GetDebugSegments()
{
    return Settings.Segments;
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogDebug::GetCircleSegments()
{
    return (Settings.Segments * 2) + 2; // because DrawDebugCircle does Segments = FMath::Max((Segments - 2) / 2, 4) for some reason
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebug::GetDebugThickness(float InThickness)
{
    return (Settings.Thickness + InThickness);
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebug::GetDebugServerThickness(float InThickness)
{
    return (Settings.ServerThickness + InThickness);
}

//--------------------------------------------------------------------------------------------------------------------------
uint8 FCogDebug::GetDebugDepthPriority(float InDepthPriority)
{
    return (Settings.DepthPriority + InDepthPriority);
}

//--------------------------------------------------------------------------------------------------------------------------
FColor FCogDebug::ModulateDebugColor(const UWorld* World, const FColor& Color, bool bPersistent)
{
    if (bPersistent == false)
    {
        return Color;
    }

    const float Time = World->GetTimeSeconds();
    const FLinearColor BaseColor(Color);
    FLinearColor ComplementaryColor = BaseColor.LinearRGBToHSV();

    ComplementaryColor.R = ComplementaryColor.R - 180.0f;
    if (ComplementaryColor.R < 0.0f)
    {
        ComplementaryColor.R = 360.0f - ComplementaryColor.R;
    }

    ComplementaryColor = ComplementaryColor.HSVToLinearRGB();

    const FLinearColor GradientColor = FLinearColor::LerpUsingHSV(FLinearColor(Color), ComplementaryColor, FMath::Cos(Settings.GradientColorSpeed * Time));
    const FLinearColor FBlendColor = BaseColor * (1.0f - Settings.GradientColorIntensity) + GradientColor * Settings.GradientColorIntensity;
    return FBlendColor.ToFColor(true);
}


//--------------------------------------------------------------------------------------------------------------------------
FColor FCogDebug::ModulateServerColor(const FColor& Color)
{
    FColor ServerColor(
        Color.R * Settings.ServerColorMultiplier,
        Color.G * Settings.ServerColorMultiplier,
        Color.B * Settings.ServerColorMultiplier,
        Color.A);

    return ServerColor;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug::IsSecondarySkeletonBone(FName BoneName)
{
    FString BoneString = BoneName.ToString().ToLower();

    for (const FString& Wildcard : Settings.SecondaryBoneWildcards)
    {
        if (BoneString.MatchesWildcard(Wildcard))
        {
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::GetDebugChannelColors(FColor ChannelColors[ECC_MAX])
{
    ChannelColors[ECC_WorldStatic]            = Settings.ChannelColorWorldStatic;
    ChannelColors[ECC_WorldDynamic]           = Settings.ChannelColorWorldDynamic;
    ChannelColors[ECC_Pawn]                   = Settings.ChannelColorPawn;
    ChannelColors[ECC_Visibility]             = Settings.ChannelColorVisibility;
    ChannelColors[ECC_Camera]                 = Settings.ChannelColorCamera;
    ChannelColors[ECC_PhysicsBody]            = Settings.ChannelColorPhysicsBody;
    ChannelColors[ECC_Vehicle]                = Settings.ChannelColorVehicle;
    ChannelColors[ECC_Destructible]           = Settings.ChannelColorDestructible;
    ChannelColors[ECC_EngineTraceChannel1]    = Settings.ChannelColorEngineTraceChannel1;
    ChannelColors[ECC_EngineTraceChannel2]    = Settings.ChannelColorEngineTraceChannel2;
    ChannelColors[ECC_EngineTraceChannel3]    = Settings.ChannelColorEngineTraceChannel3;
    ChannelColors[ECC_EngineTraceChannel4]    = Settings.ChannelColorEngineTraceChannel4;
    ChannelColors[ECC_EngineTraceChannel5]    = Settings.ChannelColorEngineTraceChannel5;
    ChannelColors[ECC_EngineTraceChannel6]    = Settings.ChannelColorEngineTraceChannel6;
    ChannelColors[ECC_GameTraceChannel1]      = Settings.ChannelColorGameTraceChannel1;
    ChannelColors[ECC_GameTraceChannel2]      = Settings.ChannelColorGameTraceChannel2;
    ChannelColors[ECC_GameTraceChannel3]      = Settings.ChannelColorGameTraceChannel3;
    ChannelColors[ECC_GameTraceChannel4]      = Settings.ChannelColorGameTraceChannel4;
    ChannelColors[ECC_GameTraceChannel5]      = Settings.ChannelColorGameTraceChannel5;
    ChannelColors[ECC_GameTraceChannel6]      = Settings.ChannelColorGameTraceChannel6;
    ChannelColors[ECC_GameTraceChannel7]      = Settings.ChannelColorGameTraceChannel7;
    ChannelColors[ECC_GameTraceChannel8]      = Settings.ChannelColorGameTraceChannel8;
    ChannelColors[ECC_GameTraceChannel9]      = Settings.ChannelColorGameTraceChannel9;
    ChannelColors[ECC_GameTraceChannel10]     = Settings.ChannelColorGameTraceChannel10;
    ChannelColors[ECC_GameTraceChannel11]     = Settings.ChannelColorGameTraceChannel11;
    ChannelColors[ECC_GameTraceChannel12]     = Settings.ChannelColorGameTraceChannel12;
    ChannelColors[ECC_GameTraceChannel13]     = Settings.ChannelColorGameTraceChannel13;
    ChannelColors[ECC_GameTraceChannel14]     = Settings.ChannelColorGameTraceChannel14;
    ChannelColors[ECC_GameTraceChannel15]     = Settings.ChannelColorGameTraceChannel15;
    ChannelColors[ECC_GameTraceChannel16]     = Settings.ChannelColorGameTraceChannel16;
    ChannelColors[ECC_GameTraceChannel17]     = Settings.ChannelColorGameTraceChannel17;
    ChannelColors[ECC_GameTraceChannel18]     = Settings.ChannelColorGameTraceChannel18;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::GetDebugDrawOverlapSettings(FCogDebugDrawOverlapParams& Params)
{
    Params.HitColor = Settings.CollisionQueryHitColor;
    Params.NoHitColor = Settings.CollisionQueryNoHitColor;
    Params.DrawHitPrimitives = Settings.CollisionQueryDrawHitPrimitives;
    Params.DrawHitPrimitiveActorsName = Settings.CollisionQueryDrawHitPrimitiveActorsName;
    Params.HitPrimitiveActorsNameShadow = Settings.CollisionQueryHitPrimitiveActorsNameShadow;
    Params.HitPrimitiveActorsNameSize = Settings.CollisionQueryHitPrimitiveActorsNameSize;
    Params.Persistent = false;
    Params.LifeTime = 0.0f;
    Params.DepthPriority = Settings.DepthPriority;
    Params.Thickness = Settings.Thickness;

	GetDebugChannelColors(Params.ChannelColors);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::GetDebugDrawLineTraceSettings(FCogDebugDrawLineTraceParams& Params)
{
    GetDebugDrawOverlapSettings(Params);

    Params.DrawHitLocation = Settings.CollisionQueryDrawHitLocation;
    Params.DrawHitImpactPoints = Settings.CollisionQueryDrawHitImpactPoints;
    Params.DrawHitNormals = Settings.CollisionQueryDrawHitNormals;
    Params.DrawHitImpactNormals = Settings.CollisionQueryDrawHitImpactNormals;
    Params.HitPointSize = Settings.CollisionQueryHitPointSize;
    Params.NormalColor = Settings.CollisionQueryNormalColor;
    Params.ImpactNormalColor = Settings.CollisionQueryImpactNormalColor;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug::GetDebugDrawSweepSettings(FCogDebugDrawSweepParams& Params)
{
    GetDebugDrawLineTraceSettings(Params);

    Params.DrawHitShapes = Settings.CollisionQueryDrawHitShapes;
}