#include "CogSampleTask_WaitAttributeChanged.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleTask_WaitAttributeChanged* UCogSampleTask_WaitAttributeChanged::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
    if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
    {
        return nullptr;
    }

    UCogSampleTask_WaitAttributeChanged* WaitForAttributeChangedTask = NewObject<UCogSampleTask_WaitAttributeChanged>();
    WaitForAttributeChangedTask->AbilitySystemComponent = AbilitySystemComponent;
    WaitForAttributeChangedTask->AttributeToListenFor = Attribute;

    return WaitForAttributeChangedTask;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_WaitAttributeChanged::Activate()
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).AddUObject(this, &UCogSampleTask_WaitAttributeChanged::AttributeChanged);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_WaitAttributeChanged::EndTask()
{
    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
    }

    SetReadyToDestroy();
    MarkAsGarbage();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_WaitAttributeChanged::AttributeChanged(const FOnAttributeChangeData& Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}