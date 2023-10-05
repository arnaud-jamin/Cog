#include "CogSampleAttributeSet_Health.h"

#include "CogSampleCharacter.h"
#include "GameplayEffectExtension.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAttributeSet_Health::UCogSampleAttributeSet_Health()
{
    InitMaxHealth(1000.0f);
    InitHealth(1000.0f);
    InitHealthRegen(10.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::OnRep_Health(const FGameplayAttributeData& PrevHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Health, Health, PrevHealth);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::OnRep_MaxHealth(const FGameplayAttributeData& PrevMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Health, MaxHealth, PrevMaxHealth);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::OnRep_HealthRegen(const FGameplayAttributeData& PrevHealthRegen)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Health, HealthRegen, PrevHealthRegen);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::OnRep_DamageResistance(const FGameplayAttributeData& PrevDamageResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCogSampleAttributeSet_Health, DamageResistance, PrevDamageResistance);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;
    
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Health, Health, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Health, MaxHealth, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Health, HealthRegen, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Health, DamageResistance, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttributes(Attribute, NewValue);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxHealthAttribute())
    {
        UCogSampleFunctionLibrary_Gameplay::AdjustAttributeForMaxChange(GetOwningAbilitySystemComponent(), Health, OldValue, NewValue, GetHealthAttribute());
    }
    else
    {
        ClampAttributes(Attribute, NewValue);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    ACogSampleCharacter* Character = Cast<ACogSampleCharacter>(Data.Target.AbilityActorInfo->AvatarActor.Get());
    const float CurentHealth = GetHealth();

    if (CurentHealth <= 0.0f && bOutOfHealth == false)
    {
        const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
        Character->OnKilled(EffectContext.GetOriginalInstigator(), EffectContext.GetEffectCauser(), Data.EffectSpec, Data.EvaluatedData.Magnitude);
    }
    else if (CurentHealth > 0.0f && bOutOfHealth)
    {
        const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
        Character->OnRevived(EffectContext.GetOriginalInstigator(), EffectContext.GetEffectCauser(), Data.EffectSpec, Data.EvaluatedData.Magnitude);
    }

    bOutOfHealth = CurentHealth <= 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Health::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
}