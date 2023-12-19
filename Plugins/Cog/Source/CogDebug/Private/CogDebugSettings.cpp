#include "CogDebugSettings.h"

#include "CogCommonDebugFilteredActorInterface.h"
#include "CogDebugReplicator.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

//--------------------------------------------------------------------------------------------------------------------------
TWeakObjectPtr<AActor> FCogDebugSettings::Selection;
FCogDebugData FCogDebugSettings::Data = FCogDebugData();

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugSettings::Reset()
{
    Data = FCogDebugData();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::IsDebugActiveForObject(const UObject* WorldContextObject)
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

    bool Result = IsDebugActiveForObject_Internal(WorldContextObject, Selection.Get(), Data.bIsFilteringBySelection);

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::IsReplicatedDebugActiveForObject(const UObject* WorldContextObject, const AActor* ServerSelection, bool IsServerFilteringBySelection)
{
    return IsDebugActiveForObject_Internal(WorldContextObject, ServerSelection, IsServerFilteringBySelection);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::IsDebugActiveForObject_Internal(const UObject* WorldContextObject, const AActor* InSelection, bool InIsFilteringBySelection)
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
AActor* FCogDebugSettings::GetSelection()
{
    return Selection.Get();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugSettings::SetSelection(UWorld* World, AActor* Value)
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
bool FCogDebugSettings::GetIsFilteringBySelection()
{
    return Data.bIsFilteringBySelection;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugSettings::SetIsFilteringBySelection(UWorld* World, bool Value)
{
    Data.bIsFilteringBySelection = Value;

    if (World != nullptr && World->GetNetMode() == NM_Client)
    {
        if (ACogDebugReplicator* Replicator = ACogDebugReplicator::GetLocalReplicator(*World))
        {
            Replicator->Server_SetIsFilteringBySelection(Value);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::GetDebugPersistent(bool bPersistent)
{
    return Data.Persistent && bPersistent;
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugDuration(bool bPersistent)
{
    return bPersistent == false ? 0.0f : Data.Duration;
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugTextDuration(bool bPersistent)
{
    if (bPersistent)
    {
        return Data.Persistent ? 100 : Data.Duration;
    }
    else
    {
        return 0.0f;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogDebugSettings::GetDebugSegments()
{
    return Data.Segments;
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogDebugSettings::GetCircleSegments()
{
    return (Data.Segments * 2) + 2; // because DrawDebugCircle does Segments = FMath::Max((Segments - 2) / 2, 4) for some reason
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugThickness(float InThickness)
{
    return (Data.Thickness + InThickness);
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugServerThickness(float InThickness)
{
    return (Data.ServerThickness + InThickness);
}

//--------------------------------------------------------------------------------------------------------------------------
uint8 FCogDebugSettings::GetDebugDepthPriority(float InDepthPriority)
{
    return (Data.DepthPriority + InDepthPriority);
}

//--------------------------------------------------------------------------------------------------------------------------
FColor FCogDebugSettings::ModulateDebugColor(const UWorld* World, const FColor& Color, bool bPersistent)
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

    const FLinearColor GradientColor = FLinearColor::LerpUsingHSV(FLinearColor(Color), ComplementaryColor, FMath::Cos(Data.GradientColorSpeed * Time));
    const FLinearColor FBlendColor = BaseColor * (1.0f - Data.GradientColorIntensity) + GradientColor * Data.GradientColorIntensity;
    return FBlendColor.ToFColor(true);
}


//--------------------------------------------------------------------------------------------------------------------------
FColor FCogDebugSettings::ModulateServerColor(const FColor& Color)
{
    FColor ServerColor(
        Color.R * Data.ServerColorMultiplier,
        Color.G * Data.ServerColorMultiplier,
        Color.B * Data.ServerColorMultiplier,
        Color.A);

    return ServerColor;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::IsSecondarySkeletonBone(FName BoneName)
{
    FString BoneString = BoneName.ToString().ToLower();

    for (const FString& Wildcard : Data.SecondaryBoneWildcards)
    {
        if (BoneString.MatchesWildcard(Wildcard))
        {
            return true;
        }
    }

    return false;
}
