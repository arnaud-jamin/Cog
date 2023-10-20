#include "CogSampleFunctionLibrary_Gameplay.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogSampleGameplayEffectContext.h"
#include "CogSampleTargetableInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameplayCueNotifyTypes.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "ScalableFloat.h"

#if ENABLE_COG
#include "CogDebugDraw.h"
#include "CogDebugLog.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::AdjustAttributeForMaxChange(UAbilitySystemComponent* AbilityComponent, FGameplayAttributeData& AffectedAttribute, float OldValue, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
    if (AbilityComponent == nullptr)
    {
        return;
    }

    if (FMath::IsNearlyEqual(OldValue, NewMaxValue))
    {
        return;
    }

    // Change current value to maintain the current Val / Max percent
    const float CurrentValue = AffectedAttribute.GetCurrentValue();
    const float NewDelta = (OldValue > 0.f) ? (CurrentValue * NewMaxValue / OldValue) - CurrentValue : NewMaxValue;

    AbilityComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector UCogSampleFunctionLibrary_Gameplay::GetActorBottomLocation(const AActor* Actor)
{
    const FVector Location = Actor->GetActorLocation();

    if (const ACharacter* Character = Cast<ACharacter>(Actor))
    {
        const UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
        const FVector BottomLocation = Location - FVector::UpVector * CapsuleComponent->GetScaledCapsuleHalfHeight();
        return BottomLocation;
    }

    return Location;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Gameplay::FindAbilitySpecHandleFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayAbilitySpecHandle& SpecHandle)
{
    if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass))
    {
        SpecHandle = Spec->Handle;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::ApplyAllGameplayEffectContextValues(const FGameplayCueParameters& Parameters, const FGameplayCueNotify_SpawnResult& SpawnResult)
{
    TArray<FCogSampleGameplayEffectContextFloatValue> Entries;
    UCogSampleEffectContextLibrary::EffectContextGetAllFloatValues(Parameters.EffectContext, Entries);

    for (TObjectPtr<UFXSystemComponent> FXSystemComponent : SpawnResult.FxSystemComponents)
    {
        if (UFXSystemComponent* FXSystemComponentPtr = FXSystemComponent.Get())
        {
            for (const FCogSampleGameplayEffectContextFloatValue& Entry : Entries)
            {
                FXSystemComponentPtr->SetFloatParameter(Entry.Name, Entry.Value);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::SendMontageEvent(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)
{
    if (::IsValid(Actor))
    {
        UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
        if (AbilitySystemComponent != nullptr && IsValidChecked(AbilitySystemComponent))
        {
            FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
            AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleFunctionLibrary_Gameplay::GetFloatValue(const FScalableFloat& ScalableFloat, int32 Level)
{
    return ScalableFloat.GetValueAtLevel(Level);
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Gameplay::GetBoolValue(const FScalableFloat& ScalableFloat, int32 Level)
{
    return ScalableFloat.GetValueAtLevel(Level) > 0;
}

//--------------------------------------------------------------------------------------------------------------------------
int32 UCogSampleFunctionLibrary_Gameplay::GetIntValue(const FScalableFloat& ScalableFloat, int32 Level)
{
    return (int32)ScalableFloat.GetValueAtLevel(Level);
}

//--------------------------------------------------------------------------------------------------------------------------
FCollisionObjectQueryParams UCogSampleFunctionLibrary_Gameplay::ConfigureCollisionObjectParams(const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes)
{
    TArray<TEnumAsByte<ECollisionChannel>> CollisionObjectTraces;
    CollisionObjectTraces.AddUninitialized(ObjectTypes.Num());

    for (auto Iter = ObjectTypes.CreateConstIterator(); Iter; ++Iter)
    {
        CollisionObjectTraces[Iter.GetIndex()] = UEngineTypes::ConvertToCollisionChannel(*Iter);
    }

    FCollisionObjectQueryParams ObjectParams;
    for (auto Iter = CollisionObjectTraces.CreateConstIterator(); Iter; ++Iter)
    {
        const ECollisionChannel& Channel = (*Iter);
        if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
        {
            ObjectParams.AddObjectTypesToQuery(Channel);
        }
    }

    return ObjectParams;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(const AActor* Actor)
{
    if (Actor == nullptr)
    {
        return FVector::ZeroVector;
    }

    if (const ICogSampleTargetableInterface* Targetable = Cast<ICogSampleTargetableInterface>(Actor))
    {
        return Targetable->GetTargetLocation();
    }

    return Actor->GetActorLocation();
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleFunctionLibrary_Gameplay::AngleBetweenVector2D(FVector2D A, FVector2D B)
{
    A.Normalize();
    B.Normalize();
    return FMath::RadiansToDegrees(FMath::Acos(FVector2D::DotProduct(A, B)));
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::FindSegmentPointDistance(const FVector2D& Segment1, const FVector2D& Segment2, const FVector2D& Point, FVector2D& Projection, float& Time, float& Distance)
{
    const float DistSquared = FVector2D::DistSquared(Segment1, Segment2);
    if (FMath::IsNearlyZero(DistSquared))
    {
        Time = 0.0f;
        Projection = Segment1;
        Distance = FVector2D::Distance(Point, Segment1);
    }
    else
    {
        Time = FMath::Max(0.0f, FMath::Min(1.0f, FVector2D::DotProduct(Point - Segment1, Segment2 - Segment1) / DistSquared));
        Projection = Segment1 + Time * (Segment2 - Segment1);
        Distance = FVector2D::Distance(Point, Projection);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::FindCapsulePointDistance(const FVector2D& CapsulePoint1, const FVector2D& CapsulePoint2, const float CapsuleRadius, const FVector2D& Point, FVector2D& Projection, float& Time, float& Distance)
{
    FindSegmentPointDistance(CapsulePoint1, CapsulePoint2, Point, Projection, Time, Distance);

    float projectionToPointDistance = Distance;
    Distance -= CapsuleRadius;

    if (Distance > 0.0f)
    {
        Projection = Point + ((Projection - Point) / projectionToPointDistance) * Distance;
    }
    else
    {
        Projection = Point;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D UCogSampleFunctionLibrary_Gameplay::ViewportToScreen(const FVector2D& value, const FVector2D& displaySize)
{
    FVector2D result;

    if (displaySize.X > displaySize.Y)
    {
        const float screenXStart = (displaySize.X - displaySize.Y) * 0.5f;
        const float screenXEnd = displaySize.X - screenXStart;
        result.X = UKismetMathLibrary::MapRangeUnclamped(value.X, -1.0f, 1.0f, screenXStart, screenXEnd);
        result.Y = UKismetMathLibrary::MapRangeUnclamped(value.Y, -1.0f, 1.0f, displaySize.Y, 0.0f);
    }
    else
    {
        const float ScreenYStart = (displaySize.Y - displaySize.X) * 0.5f;
        const float ScreenYEnd = displaySize.Y - ScreenYStart;
        result.X = UKismetMathLibrary::MapRangeUnclamped(value.X, -1.0f, 1.0f, 0.0f, displaySize.X);
        result.Y = UKismetMathLibrary::MapRangeUnclamped(value.Y, -1.0f, 1.0f, ScreenYEnd, ScreenYStart);
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleFunctionLibrary_Gameplay::ViewportToScreen(const float Value, const FVector2D& DisplaySize)
{
    return Value * 0.5f * FMath::Min(DisplaySize.X, DisplaySize.Y);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D UCogSampleFunctionLibrary_Gameplay::ScreenToViewport(const FVector2D& Value, const FVector2D& DisplaySize)
{
    FVector2D Result;

    if (DisplaySize.X > DisplaySize.Y)
    {
        const float screenXStart = (DisplaySize.X - DisplaySize.Y) * 0.5f;
        const float screenXEnd = DisplaySize.X - screenXStart;
        Result.X = UKismetMathLibrary::MapRangeUnclamped(Value.X, screenXStart, screenXEnd, -1.0f, 1.0f);
        Result.Y = UKismetMathLibrary::MapRangeUnclamped(Value.Y, 0.0f, DisplaySize.Y, -1.0f, 1.0f);
    }
    else
    {
        const float screenYStart = (DisplaySize.Y - DisplaySize.X) * 0.5f;
        const float screenYEnd = DisplaySize.Y - screenYStart;
        Result.X = UKismetMathLibrary::MapRangeUnclamped(Value.X, 0.0f, DisplaySize.X, -1.0f, 1.0f);
        Result.Y = UKismetMathLibrary::MapRangeUnclamped(Value.Y, screenYStart, screenYEnd, -1.0f, 1.0f);
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleFunctionLibrary_Gameplay::ScreenToViewport(const float Value, const FVector2D& DisplaySize)
{
    return Value * 2.0f / FMath::Min(DisplaySize.X, DisplaySize.Y);
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Gameplay::HasLineOfSight(
    const UWorld* World,
    const FVector& Start,
    const FVector& End,
    const FCollisionObjectQueryParams& BlockersParams,
    const FCollisionQueryParams& QueryParams,
    const COG_LOG_CATEGORY& LogCategory)
{
    IF_COG(FCogDebugDraw::Sphere(LogCategory, World, Start, 5.0f, FColor::Black, false, 0));
    IF_COG(FCogDebugDraw::Sphere(LogCategory, World, End, 10.0f, FColor::Black, false, 0));

    FHitResult BlockerRaycastHit;
    if (World->LineTraceSingleByObjectType(BlockerRaycastHit, Start, End, BlockersParams, QueryParams))
    {
        IF_COG(FCogDebugDraw::Segment(LogCategory, World, Start, End, FColor(0, 0, 0, 10), false, 0));
        IF_COG(FCogDebugDraw::Sphere(LogCategory, World, BlockerRaycastHit.ImpactPoint, 5.0f, FColor::Red, false, 0));
    }
    else
    {
        IF_COG(FCogDebugDraw::Segment(LogCategory, World, Start, End, FColor(255, 255, 255, 10), false, 0));

        //--------------------------------------------------------------------------------------------------------------
        // We didn't touch a blocker, we have line of sight.
        //--------------------------------------------------------------------------------------------------------------
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Gameplay::IsActorAbilitySystemMatchingTags(const UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& RequiredTags, const FGameplayTagContainer& IgnoredTags)
{
    if (AbilitySystem == nullptr)
    {
        return false;
    }

    const bool bHasRequiredTags = AbilitySystem->HasAllMatchingGameplayTags(RequiredTags);
    const bool bHasIgnoredTags = AbilitySystem->HasAnyMatchingGameplayTags(IgnoredTags);
    
    if (bHasRequiredTags && bHasIgnoredTags == false)
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Gameplay::IsActorMatchingTags(const AActor* Actor, const FGameplayTagContainer& RequiredTags, const FGameplayTagContainer& IgnoredTags)
{
    const UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
    bool Result = IsActorAbilitySystemMatchingTags(AbilitySystem, RequiredTags, IgnoredTags);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
AActor* UCogSampleFunctionLibrary_Gameplay::GetActorInstigator(AActor* Actor) 
{
    if (Actor == nullptr)
    {
        return nullptr;
    }

    AActor* Instigator = Actor->GetInstigator();
    if (Instigator != nullptr)
    {
        return Instigator;
    }

    //-------------------------------------------------
    // The game state is the default Instigator
    //-------------------------------------------------
    UWorld* World = Actor->GetWorld();
    if (World != nullptr)
    {
        return World->GetGameState();
    }

    return nullptr;
}