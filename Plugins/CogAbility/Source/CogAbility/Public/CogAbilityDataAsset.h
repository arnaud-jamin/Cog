#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "CogAbilityDataAsset.generated.h"


class UGameplayEffect;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityCheat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Cog")
    FString Name;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TSubclassOf<UGameplayEffect> Effect;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityPool
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Cog")
    FString Name;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayAttribute Value;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayAttribute Min;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayAttribute Max;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayAttribute Regen;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere, Category = "Cog")
    FLinearColor BackColor = FLinearColor(0.15, 0.15, 0.15, 1.0f);
};


//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweak
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Cog")
    FName Name;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(EditAnywhere, Category = "Cog")
    float Multiplier = 0.01f;

    UPROPERTY(EditAnywhere, Category = "Cog")
    float AddPostMultiplier = 1.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakCategory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Cog")
    FString Name;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayTag Id;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayTagContainer RequiredTags;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayTagContainer IgnoredTags;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FLinearColor Color = FLinearColor::White;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakProfileValue
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Cog")
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FGameplayTag CategoryId;

    UPROPERTY(EditAnywhere, Category = "Cog")
    float Value = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Cog")
    FName Name;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TArray<FCogAbilityTweakProfileValue> Tweaks;
};


//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGABILITY_API UCogAbilityDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    UCogAbilityDataAsset() {}

    //----------------------------------------------------------------------------------------------------------------------
    // Abilities
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category = "Abilities", EditAnywhere)
    TArray<TSubclassOf<UGameplayAbility>> Abilities;

    //----------------------------------------------------------------------------------------------------------------------
    // Cheats
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category="Cheats", EditAnywhere)
    FGameplayTag NegativeEffectTag;

    UPROPERTY(Category = "Cheats", EditAnywhere)
    FGameplayTag PositiveEffectTag;

    UPROPERTY(Category="Cheats", EditAnywhere)
    FGameplayTag NegativeAbilityTag;

    UPROPERTY(Category = "Cheats", EditAnywhere)
    FGameplayTag PositiveAbilityTag;

    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityCheat> PersistentEffects;

    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityCheat> InstantEffects;

    //----------------------------------------------------------------------------------------------------------------------
    // Pools
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category = "Pools", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityPool> Pools;

    //----------------------------------------------------------------------------------------------------------------------
    // Tweaks
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    float TweakMinValue = -100.0f;

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    float TweakMaxValue = 200.0f;

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    FGameplayTag SetByCallerMagnitudeTag;

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    TSubclassOf<AActor> ActorRootClass;

    UPROPERTY(Category = "Tweaks", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweak> Tweaks;

    UPROPERTY(Category = "Tweaks", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweakCategory> TweaksCategories;

    UPROPERTY(Category = "Tweaks", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweakProfile>  TweakProfiles;
};