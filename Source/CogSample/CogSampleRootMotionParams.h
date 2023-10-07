#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RootMotionSource.h"
#include "CogSampleRootMotionParams.generated.h"

class UGameplayEffect;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCogSampleRootMotionParams
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Instigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Causer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsAdditive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Distance = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Height = 0.0f;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinimumLandedTriggerTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFinishOnLanded = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERootMotionFinishVelocityMode FinishVelocityMode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector FinishSetVelocity = FVector::ZeroVector;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FinishClampVelocity = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCurveVector* PathOffsetCurve = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCurveFloat* TimeMappingCurve = nullptr;

};

