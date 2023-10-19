#pragma once

#include "CoreMinimal.h"
#include "CogSampleDamageableInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCogSampleDamageEventParams
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    float MitigatedDamage = 0;
    
    UPROPERTY(BlueprintReadWrite)
    float UnmitigatedDamage = 0;
    
    UPROPERTY(BlueprintReadWrite)
    AActor* DamageDealer = nullptr;
    
    UPROPERTY(BlueprintReadWrite)
    AActor* DamageReceiver = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCogSampleDamageEventDelegate, const FCogSampleDamageEventParams&, Params);

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogSampleDamageableInterface : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogSampleDamageableInterface
{
    GENERATED_BODY()

public:

    virtual void HandleDamageReceived(const FCogSampleDamageEventParams& Params) {}

    virtual void HandleDamageDealt(const FCogSampleDamageEventParams& Params) {}
};
