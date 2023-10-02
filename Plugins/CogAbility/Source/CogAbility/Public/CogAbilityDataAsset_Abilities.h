#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAbilityDataAsset_Abilities.generated.h"

class UGameplayAbility;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGABILITY_API UCogAbilityDataAsset_Abilities : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogAbilityDataAsset_Abilities() {}

    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<UGameplayAbility>> Abilities;
};
