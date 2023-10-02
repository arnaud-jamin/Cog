#include "CogSampleAttributeSet_Stamina.h"

#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAttributeSet_Stamina::UCogSampleAttributeSet_Stamina()
{
    InitMinStamina(-500.0f);
    InitStamina(1000.0f);
    InitMaxStamina(1000.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::OnRep_Stamina(const FGameplayAttributeData& PrevStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Stamina, Stamina, PrevStamina);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::OnRep_MinStamina(const FGameplayAttributeData& PrevMinStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Stamina, MinStamina, PrevMinStamina);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::OnRep_MaxStamina(const FGameplayAttributeData& PrevMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Stamina, MaxStamina, PrevMaxStamina);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::OnRep_StaminaRegen(const FGameplayAttributeData& PrevStaminaRegen)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Stamina, StaminaRegen, PrevStaminaRegen);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;
    
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Stamina, Stamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Stamina, MinStamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Stamina, MaxStamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Stamina, StaminaRegen, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, GetMinStamina(), GetMaxStamina());
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxStaminaAttribute())
    {
        UCogSampleFunctionLibrary_Gameplay::AdjustAttributeForMaxChange(GetOwningAbilitySystemComponent(), Stamina, OldValue, NewValue, GetStaminaAttribute());
    }
    else
    {
        ClampAttributes(Attribute, NewValue);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Stamina::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, GetMinStamina(), GetMaxStamina());
    }
}
