#pragma once

#include "CoreMinimal.h"
#include "CogWindowConfig.h"
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
class UCogAbilityConfig_Alignment : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    FVector4f GetAttributeColor(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute) const;

    FVector4f GetEffectModifierColor(const FModifierSpec& ModSpec, const FGameplayModifierInfo& ModInfo, float BaseValue) const;

    FVector4f GetEffectModifierColor(float ModifierValue, EGameplayModOp::Type ModifierOp, float BaseValue) const;

    FVector4f GetEffectColor(const UCogAbilityDataAsset* Asset, const UGameplayEffect& Effect) const;


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