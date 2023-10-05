#include "CogSampleAttributeSet_Caster.h"

#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAttributeSet_Caster::UCogSampleAttributeSet_Caster()
{
    InitBaseDamage(100.0f);
    InitCooldownReduction(0.0f);
    InitAreaRadiusModifier(0.0f);
    InitMinStamina(-500.0f);
    InitStamina(1000.0f);
    InitMaxStamina(1000.0f);
    InitStaminaRegen(1.0f);
    InitStaminaCostReduction(0.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;

    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, BaseDamage, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, CooldownReduction, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, AreaRadiusModifier, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, Stamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, MinStamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, MaxStamina, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, StaminaRegen, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Caster, StaminaCostReduction, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_BaseDamage(const FGameplayAttributeData& PrevBaseDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, BaseDamage, PrevBaseDamage);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_CooldownReduction(const FGameplayAttributeData& PrevCooldownReduction)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, CooldownReduction, PrevCooldownReduction);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_AreaRadiusModifier(const FGameplayAttributeData& PrevAreaRadiusModifier)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, AreaRadiusModifier, PrevAreaRadiusModifier);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_Stamina(const FGameplayAttributeData& PrevStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, Stamina, PrevStamina);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_MinStamina(const FGameplayAttributeData& PrevMinStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, MinStamina, PrevMinStamina);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_MaxStamina(const FGameplayAttributeData& PrevMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, MaxStamina, PrevMaxStamina);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_StaminaRegen(const FGameplayAttributeData& PrevStaminaRegen)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, StaminaRegen, PrevStaminaRegen);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::OnRep_StaminaCostReduction(const FGameplayAttributeData& PrevStaminaCostReduction)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Caster, StaminaCostReduction, PrevStaminaCostReduction);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

    if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, GetMinStamina(), GetMaxStamina());
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
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
void UCogSampleAttributeSet_Caster::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Caster::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, GetMinStamina(), GetMaxStamina());
    }
}