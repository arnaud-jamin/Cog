#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "Abilities/GameplayAbility.h"
#include "CogAbilityConfig_Alignment.generated.h"

class UAbilitySystemComponent;
class UCogAbilityDataAsset;
class UGameplayEffect;
namespace EGameplayModOp { enum Type : int; };
struct FGameplayAttribute;
struct FGameplayModifierInfo;
struct FModifierSpec;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Alignment : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    FVector4f GetAttributeColor(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute) const;

    bool GetAbilityColor(const UCogAbilityDataAsset* Asset, const UGameplayAbility& Ability, FLinearColor& OutColor) const;

    FVector4f GetEffectModifierColor(const FModifierSpec& ModSpec, const FGameplayModifierInfo& ModInfo, float BaseValue) const;

    FVector4f GetEffectModifierColor(float ModifierValue, EGameplayModOp::Type ModifierOp, float BaseValue) const;

    bool GetEffectColor(const UCogAbilityDataAsset* Asset, const UGameplayEffect& Effect, FLinearColor& OutColor) const;

    UPROPERTY(Config)
    FVector4f PositiveColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);

    UPROPERTY(Config)
    FVector4f NegativeColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(Config)
    FVector4f NeutralColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);

    virtual void Reset() override
    {
        Super::Reset();

        PositiveColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
        NegativeColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);
        NeutralColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
};