#include "CogSampleAbilityTask_PlayMontageAndWaitForEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "CogSampleDefines.h"
#include "CogSampleLogCategories.h"
#include "GameFramework/Character.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAbilityTask_PlayMontageAndWaitForEvent::UCogSampleAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Rate = 1.f;
    bStopWhenAbilityEnds = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s - Interrupted:%d"), *Montage->GetFName().ToString(), bInterrupted);

    if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
    {
        if (Montage == MontageToPlay)
        {
            AbilitySystemComponent->ClearAnimatingAbility(Ability);

            // Reset AnimRootMotionTranslationScale
            ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
            if (Character && (Character->GetLocalRole() == ROLE_Authority ||
                (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::Predicted)))
            {
                Character->SetAnimRootMotionTranslationScale(1.f);
            }

        }
    }

    if (bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
    else
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s"), *GetNameSafe(MontageToPlay));

    // TODO: Merge this fix back to engine, it was calling the wrong callback

    if (StopPlayingMontage())
    {
        // Let the BP handle the interrupt as well
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s - Interrupted:%d"), *Montage->GetFName().ToString(), bInterrupted);

    if (!bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }

    EndTask();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
    if (ShouldBroadcastAbilityTaskDelegates() == false)
    {
        COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s - Event:%s - ShouldBroadcastAbilityTaskDelegates:false"), *GetNameSafe(MontageToPlay), *EventTag.ToString());
        return;
    }

    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s - Event:%s"), *GetNameSafe(MontageToPlay), *EventTag.ToString());

    FGameplayEventData TempData = *Payload;
    TempData.EventTag = EventTag;
    EventReceived.Broadcast(EventTag, TempData);
}

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAbilityTask_PlayMontageAndWaitForEvent* UCogSampleAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
    FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, OwningAbility, TEXT("Montage:%s"), *GetNameSafe(MontageToPlay));

    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

    UCogSampleAbilityTask_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UCogSampleAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
    MyObj->MontageToPlay = MontageToPlay;
    MyObj->EventTags = EventTags;
    MyObj->Rate = Rate;
    MyObj->StartSection = StartSection;
    MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
    MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

    return MyObj;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
    if (Ability == nullptr)
    {
        return;
    }

    bool bPlayedMontage = false;

    if (AbilitySystemComponent.IsValid())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if (AnimInstance != nullptr)
        {
            // Bind to event callback
            EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));

            if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
            {
                // Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
                if (ShouldBroadcastAbilityTaskDelegates() == false)
                {
                    return;
                }

                CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);

                BlendingOutDelegate.BindUObject(this, &UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
                AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

                MontageEndedDelegate.BindUObject(this, &UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

                ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
                if (Character && (Character->GetLocalRole() == ROLE_Authority ||
                    (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::Predicted)))
                {
                    Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
                }

                bPlayedMontage = true;
            }
        }
        else
        {
            COG_LOG_ABILITY(ELogVerbosity::Warning, Ability, TEXT("Montage:%s - PlayMontage failed."), *GetNameSafe(MontageToPlay));
        }
    }
    else
    {
        COG_LOG_ABILITY(ELogVerbosity::Warning, Ability, TEXT("Montage:%s - Invalid AbilitySystemComponent"), *GetNameSafe(MontageToPlay));
    }

    if (!bPlayedMontage)
    {
        COG_LOG_ABILITY(ELogVerbosity::Warning, Ability, TEXT("Montage:%s - Failed to play montage."), *GetNameSafe(MontageToPlay));
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            //ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
            OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }

    SetWaitingOnAvatar();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
    check(AbilitySystemComponent.IsValid());

    OnAbilityCancelled();

    Super::ExternalCancel();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s - AbilityEnded:%d"), *GetNameSafe(MontageToPlay), AbilityEnded);

    // Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
    // (If we are destroyed, it will detect this and not do anything)

    // This delegate, however, should be cleared as it is a multicast
    if (Ability)
    {
        Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
        if (AbilityEnded && bStopWhenAbilityEnds)
        {
            StopPlayingMontage();
        }
    }

    if (AbilitySystemComponent.IsValid())
    {
        AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
    }

    Super::OnDestroy(AbilityEnded);

}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
    COG_LOG_ABILITY(ELogVerbosity::VeryVerbose, Ability, TEXT("Montage:%s"), *GetNameSafe(MontageToPlay));

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    if (!ActorInfo)
    {
        return false;
    }

    UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
    if (AnimInstance == nullptr)
    {
        return false;
    }

    // Check if the montage is still playing
    // The ability would have been interrupted, in which case we should automatically stop the montage
    if (AbilitySystemComponent.IsValid() && Ability)
    {
        if (AbilitySystemComponent->GetAnimatingAbility() == Ability
            && AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
        {
            // Unbind delegates so they don't get called as well
            FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
            if (MontageInstance)
            {
                MontageInstance->OnMontageBlendingOutStarted.Unbind();
                MontageInstance->OnMontageEnded.Unbind();
            }

            AbilitySystemComponent->CurrentMontageStop();
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogSampleAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
    UAnimMontage* PlayingMontage = nullptr;
    if (Ability)
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

        if (AnimInstance != nullptr)
        {
            PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
        }
    }

    return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}



