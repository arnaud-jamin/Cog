#include "CogSampleAttributeSet_Speed.h"

#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAttributeSet_Speed::UCogSampleAttributeSet_Speed()
{
    InitSpeed(600.0f);
    InitMinSpeed(0.0f);
    InitMaxSpeed(5000.0f);
    InitMaxAcceleration(2000.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::OnRep_Speed(const FGameplayAttributeData& PrevSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Speed, Speed, PrevSpeed);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::OnRep_MinSpeed(const FGameplayAttributeData& PrevMinSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Speed, MinSpeed, PrevMinSpeed);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::OnRep_MaxSpeed(const FGameplayAttributeData& PrevMaxSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Speed, MaxSpeed, PrevMaxSpeed);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::OnRep_MaxAcceleration(const FGameplayAttributeData& PrevMaxAcceleration)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Speed, MaxAcceleration, PrevMaxAcceleration);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;
    
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Speed, Speed, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Speed, MinSpeed, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Speed, MaxSpeed, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Speed, MaxAcceleration, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Speed::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetSpeedAttribute())
    {
        NewValue = FMath::Clamp(NewValue, GetMinSpeed(), GetMaxSpeed());
    }
}