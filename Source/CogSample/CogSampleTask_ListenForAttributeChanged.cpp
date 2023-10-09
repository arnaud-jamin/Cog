#include "CogSampleTask_ListenForAttributeChanged.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleTask_ListenForAttributeChanged* UCogSampleTask_ListenForAttributeChanged::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
    if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
    {
        return nullptr;
    }

    UCogSampleTask_ListenForAttributeChanged* WaitForAttributeChangedTask = NewObject<UCogSampleTask_ListenForAttributeChanged>();
    WaitForAttributeChangedTask->AbilitySystemComponent = AbilitySystemComponent;
    WaitForAttributeChangedTask->AttributeToListenFor = Attribute;

    return WaitForAttributeChangedTask;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForAttributeChanged::Activate()
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).AddUObject(this, &UCogSampleTask_ListenForAttributeChanged::AttributeChanged);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForAttributeChanged::EndTask()
{
    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
    }

    SetReadyToDestroy();
    MarkAsGarbage();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForAttributeChanged::AttributeChanged(const FOnAttributeChangeData& Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}