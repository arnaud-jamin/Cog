#pragma once

#include "CoreMinimal.h"
#include "CogAbilityDamageActorInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityDamageParams
{
    GENERATED_BODY()

    TObjectPtr<AActor> DamageDealer;
    TObjectPtr<AActor> DamageReceiver;
    float ReceivedDamage = 0;
    float IncomingDamage = 0;
    bool IsCritical = false;
};

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_OneParam(FCogAbilityOnDamageEvent, const FCogAbilityDamageParams&);

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogAbilityDamageActorInterface : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogAbilityDamageActorInterface
{
    GENERATED_BODY()

public:
    virtual FCogAbilityOnDamageEvent& OnDamageEvent()  = 0;
};