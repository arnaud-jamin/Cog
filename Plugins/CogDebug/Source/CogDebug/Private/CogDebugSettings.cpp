#include "CogDebugSettings.h"

#include "CogInterfaceDebugFilteredActor.h"

//--------------------------------------------------------------------------------------------------------------------------
TWeakObjectPtr<AActor> FCogDebugSettings::Selection;
bool FCogDebugSettings::FilterBySelection = true;
bool FCogDebugSettings::Persistent = false;
bool FCogDebugSettings::TextShadow = true;
bool FCogDebugSettings::Fade2D = true;
float FCogDebugSettings::Duration = 3.0f;
int FCogDebugSettings::DepthPriority = 0;
int FCogDebugSettings::Segments = 12;
float FCogDebugSettings::Thickness = 0.0f;
float FCogDebugSettings::ServerThickness = 2.0f;
float FCogDebugSettings::ServerColorMultiplier = 0.8f;
float FCogDebugSettings::ArrowSize = 10.0f;
float FCogDebugSettings::AxesScale = 1.0f;
float FCogDebugSettings::GradientColorIntensity = 0.0f;
float FCogDebugSettings::GradientColorSpeed = 2.0f;
float FCogDebugSettings::TextSize = 1.0f;
TArray<FString> FCogDebugSettings::SecondaryBoneWildcards = 
{ 
    "interaction", 
    "center_of_mass", 
    "ik_*", 
    "index_*", 
    "middle_*",  
    "pinky_*", 
    "ring_*", 
    "thumb_*", 
    "wrist_*", 
    "*_bck_*",
    "*_fwd_*",
    "*_in_*",
    "*_out_*",
    "*_pec_*",
    "*_scap_*",
    "*_bicep_*",
    "*_tricep_*",
    "*ankle*",
    "*knee*", 
    "*corrective*",
    "*twist*",
    "*latissimus*",
};

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugSettings::Reset()
{
    FilterBySelection = true;
    Persistent = false;
    TextShadow = true;
    Fade2D = true;
    Duration = 3.0f;
    DepthPriority = 0;
    Segments = 12;
    Thickness = 0.0f;
    ServerThickness = 2.0f;
    ServerColorMultiplier = 0.8f;
    ArrowSize = 10.0f;
    AxesScale = 1.0f;
    GradientColorIntensity = 0.0f;
    GradientColorSpeed = 2.0f;
    TextSize = 1.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::IsDebugActiveForObject(const UObject* WorldContextObject)
{
    if (FilterBySelection == false)
    {
        return true;
    }

    if (WorldContextObject == nullptr)
    {
        return true;
    }

    const AActor* SelectionPtr = Selection.Get();
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

        if (Cast<ICogInterfacesDebugFilteredActor>(Outer))
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
void FCogDebugSettings::SetSelection(AActor* Value)
{
    Selection = Value;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::GetDebugPersistent(bool bPersistent)
{
    return Persistent && bPersistent;
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugDuration(bool bPersistent)
{
    return bPersistent == false ? 0.0f : Duration;
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugTextDuration(bool bPersistent)
{
    if (bPersistent)
    {
        return Persistent ? 100 : Duration;
    }
    else
    {
        return 0.0f;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogDebugSettings::GetDebugSegments()
{
    return Segments;
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogDebugSettings::GetCircleSegments()
{
    return (Segments * 2) + 2; // because DrawDebugCircle does Segments = FMath::Max((Segments - 2) / 2, 4) for some reason
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugThickness(float InThickness)
{
    return (Thickness + InThickness);
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugSettings::GetDebugServerThickness(float InThickness)
{
    return (ServerThickness + InThickness);
}

//--------------------------------------------------------------------------------------------------------------------------
uint8 FCogDebugSettings::GetDebugDepthPriority(float InDepthPriority)
{
    return (DepthPriority + InDepthPriority);
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

    const FLinearColor GradientColor = FLinearColor::LerpUsingHSV(FLinearColor(Color), ComplementaryColor, FMath::Cos(GradientColorSpeed * Time));
    const FLinearColor FBlendColor = BaseColor * (1.0f - FCogDebugSettings::GradientColorIntensity) + GradientColor * GradientColorIntensity;
    return FBlendColor.ToFColor(true);
}


//--------------------------------------------------------------------------------------------------------------------------
FColor FCogDebugSettings::ModulateServerColor(const FColor& Color)
{
    FColor ServerColor(
        Color.R * ServerColorMultiplier,
        Color.G * ServerColorMultiplier,
        Color.B * ServerColorMultiplier,
        Color.A);

    return ServerColor;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugSettings::IsSecondarySkeletonBone(FName BoneName)
{
    FString BoneString = BoneName.ToString().ToLower();

    for (const FString& Wildcard : SecondaryBoneWildcards)
    {
        if (BoneString.MatchesWildcard(Wildcard))
        {
            return true;
        }
    }

    return false;
}
