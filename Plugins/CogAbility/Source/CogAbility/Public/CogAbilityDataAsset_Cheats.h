#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "CogAbilityDataAsset_Cheats.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityCheat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGABILITY_API UCogAbilityDataAsset_Cheats : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogAbilityDataAsset_Cheats() {}

    UPROPERTY(EditAnywhere)
    FGameplayTag NegativeEffectTag;
    
    UPROPERTY(EditAnywhere)
    FGameplayTag PositiveEffectTag;

    UPROPERTY(EditAnywhere)
    FLinearColor NeutralEffectColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere)
    FLinearColor NegativeEffectColor = FLinearColor(1.0f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere)
    FLinearColor PositiveEffectColor = FLinearColor(0.0f, 1.0f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityCheat> PersistentEffects;

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityCheat> InstantEffects;
};