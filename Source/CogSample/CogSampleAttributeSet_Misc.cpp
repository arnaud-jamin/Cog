#include "CogSampleAttributeSet_Misc.h"

#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------

UCogSampleAttributeSet_Misc::UCogSampleAttributeSet_Misc()
{
    InitScale(1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::OnRep_Scale(const FGameplayAttributeData& PrevScale)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Misc, Scale, PrevScale);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;
    
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Misc, Scale, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Misc::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetScaleAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.15f, 10.0f);
    }
}