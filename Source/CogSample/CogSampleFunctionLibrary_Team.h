#pragma once

#include "CoreMinimal.h"
#include "CogSampleFunctionLibrary_Team.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogSampleAllegiance : uint8
{
    Friendly,
    Neutral,
    Enemy,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECogSampleAllegianceFilter : uint8
{
    None        = 0 UMETA(Hidden),
    Friendly    = 1 << 0,
    Neutral     = 1 << 1,
    Enemy       = 1 << 2,
};
ENUM_CLASS_FLAGS(ECogSampleAllegianceFilter);

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogSampleFunctionLibrary_Team"))
class UCogSampleFunctionLibrary_Team : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure)
    static int32 GetTeamSafe(const AActor* Actor);

    UFUNCTION(BlueprintPure)
    static ECogSampleAllegiance GetTeamsAllegiance(int32 Team1, int32 Team2);

    UFUNCTION(BlueprintPure)
    static ECogSampleAllegiance GetActorsAllegiance(const AActor* Actor1, const AActor* Actor2);

    UFUNCTION(BlueprintPure)
    static bool MatchAllegiance(const AActor* Actor1, const AActor* Actor2, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter")) int32 AllegianceFilter);

    UFUNCTION(BlueprintPure)
    bool MatchAllegianceFromTeams(int32 Team1, int32 Team2, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter")) int32 AllegianceFilter);

    UFUNCTION(BlueprintPure)
    static bool MatchAllegianceBetweenTeamAndActor(int32 Team, const AActor* Actor, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter")) int32 AllegianceFilter);

    UFUNCTION(BlueprintPure)
    static bool MatchAllegianceFilter(ECogSampleAllegiance Allegiance, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter")) int32 AllegianceFilter);
    
};
