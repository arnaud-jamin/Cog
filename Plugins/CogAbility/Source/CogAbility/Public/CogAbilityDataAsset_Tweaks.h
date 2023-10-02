#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAbilityDataAsset_Tweaks.generated.h"

class UGameplayEffect;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweak
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName Name;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(EditAnywhere)
    float Multiplier = 0.01f;

    UPROPERTY(EditAnywhere)
    float AddPostMultiplier = 1.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakCategory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name;

    UPROPERTY(EditAnywhere)
    FGameplayTag Id;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(EditAnywhere)
    FGameplayTagContainer RequiredTags;

    UPROPERTY(EditAnywhere)
    FGameplayTagContainer IgnoredTags;

    UPROPERTY(EditAnywhere)
    FLinearColor Color = FLinearColor::White;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakProfileValue
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(EditAnywhere)
    FGameplayTag CategoryId;

    UPROPERTY(EditAnywhere)
    float Value = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName Name;

    UPROPERTY(EditAnywhere)
    TArray<FCogAbilityTweakProfileValue> Tweaks;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGABILITY_API UCogAbilityDataAsset_Tweaks : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UCogAbilityDataAsset_Tweaks() {}

    UPROPERTY(EditAnywhere)
    float TweakMinValue = -100.0f;

    UPROPERTY(EditAnywhere)
    float TweakMaxValue = 200.0f;

    UPROPERTY(EditAnywhere)
    FGameplayTag SetByCallerMagnitudeTag;

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweak> Tweaks;

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweakCategory> TweaksCategories;

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweakProfile>  TweakProfiles;
};
