#pragma once

#include "CoreMinimal.h"
#include "CogInterfaceDamageActor.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogInterfaceDamageEventType : uint8
{
    DamageDealt,
    DamageReceived,
};


//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogInterfaceDamageParams
{
    GENERATED_BODY()

    ECogInterfaceDamageEventType Type;
    TObjectPtr<AActor> DamageDealer;
    TObjectPtr<AActor> DamageReceiver;
    float MitigatedDamage = 0;
    float IncomingDamage = 0;
    bool IsCritical = false;
};

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_OneParam(FCogInterfaceOnDamageEvent, const FCogInterfaceDamageParams&);

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogInterfaceDamageActor : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogInterfaceDamageActor
{
    GENERATED_BODY()

public:
    virtual FCogInterfaceOnDamageEvent& OnDamageEvent()  = 0;
};