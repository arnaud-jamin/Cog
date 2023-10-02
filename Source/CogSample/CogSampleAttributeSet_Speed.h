#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleAttributeSet_Speed.generated.h"

UCLASS()
class UCogSampleAttributeSet_Speed : public UAttributeSet
{
    GENERATED_BODY()

public:

    UCogSampleAttributeSet_Speed();

    UFUNCTION()
    virtual void OnRep_Speed(const FGameplayAttributeData& PrevSpeed);		

    UFUNCTION()
    virtual void OnRep_MinSpeed(const FGameplayAttributeData& PrevMinSpeed);

    UFUNCTION()
    virtual void OnRep_MaxSpeed(const FGameplayAttributeData& PrevMaxSpeed);

    UFUNCTION()
    virtual void OnRep_MaxAcceleration(const FGameplayAttributeData& PrevMaxAcceleration);

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:

    UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_Speed, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_MinSpeed, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MinSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_MaxSpeed, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MaxSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_MaxAcceleration, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MaxAcceleration;

    void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Speed, Speed);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Speed, MinSpeed);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Speed, MaxSpeed);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Speed, MaxAcceleration);

};

