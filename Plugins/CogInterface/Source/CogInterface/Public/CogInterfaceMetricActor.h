#pragma once

#include "CoreMinimal.h"
#include "CogInterfaceMetricActor.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogInterfaceMetricEventParams
{
    GENERATED_BODY()

    FName Name;
    
    float MitigatedValue = 0;
    
    float UnmitigatedValue = 0;

    bool IsCritical = false;
};

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_OneParam(FCogInterfaceOnMetricEvent, const FCogInterfaceMetricEventParams&);

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogInterfaceMetricActor : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogInterfaceMetricActor
{
    GENERATED_BODY()

public:
    virtual FCogInterfaceOnMetricEvent& OnMetricEvent()  = 0;
};