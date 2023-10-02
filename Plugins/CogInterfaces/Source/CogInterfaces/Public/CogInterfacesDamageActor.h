#pragma once

#include "CoreMinimal.h"
#include "CogInterfacesDamageActor.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogInterfacesDamageEventType : uint8
{
    DamageDealt,
    DamageReceived,
};


//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogInterfacesDamageParams
{
    GENERATED_BODY()

    ECogInterfacesDamageEventType Type;
    TObjectPtr<AActor> DamageDealer;
    TObjectPtr<AActor> DamageReceiver;
    float MitigatedDamage = 0;
    float IncomingDamage = 0;
    bool IsCritical = false;
};

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_OneParam(FCogAbilityOnDamageEvent, const FCogInterfacesDamageParams&);

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogInterfacesDamageActor : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogInterfacesDamageActor
{
    GENERATED_BODY()

public:
    virtual FCogAbilityOnDamageEvent& OnDamageEvent()  = 0;
};