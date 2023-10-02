#pragma once

#include "CoreMinimal.h"
#include "CogSampleFunctionLibrary_Gameplay.generated.h"

class UAbilitySystemComponent;
struct FGameplayAttribute;
struct FGameplayAttributeData;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

UCLASS(meta = (ScriptName = "CogSampleFunctionLibrary_Gameplay"))
class UCogSampleFunctionLibrary_Gameplay : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static void AdjustAttributeForMaxChange(UAbilitySystemComponent* AbilityComponent, FGameplayAttributeData& AffectedAttribute, float OldValue, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
};
