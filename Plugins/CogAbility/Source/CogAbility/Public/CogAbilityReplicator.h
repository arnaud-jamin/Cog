#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogAbilityReplicator.generated.h"

class AActor;
class UAbilitySystemComponent;
class UCogAbilityDataAsset;
struct FCogAbilityCheat;
struct FCogAbilityTweak;
struct FCogAbilityTweakCategory;
struct FGameplayTag;

UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient, Config = Cog)
class COGABILITY_API ACogAbilityReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:

    static ACogAbilityReplicator* Spawn(APlayerController* Controller);

    static ACogAbilityReplicator* GetFirstReplicator(const UWorld& World);

    static void TryApplyAllTweaksOnActor(const AActor* Actor);

    static void GetReplicators(const UWorld& World, TArray<ACogAbilityReplicator*>& Replicators);

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    UFUNCTION(Reliable, Server)
    void Server_ApplyCheat(const AActor* CheatInstigator, const TArray<AActor*>& TargetActors, const FCogAbilityCheat& Cheat) const;

    static bool IsCheatActive(const AActor* EffectTarget, const FCogAbilityCheat& Cheat);

    UFUNCTION(Reliable, Server)
    void Server_GiveAbility(AActor* TargetActor, TSubclassOf<UGameplayAbility> AbilityClass) const;

    UFUNCTION(Reliable, Server)
    void Server_RemoveAbility(AActor* TargetActor, const FGameplayAbilitySpecHandle& Handle) const;

    UFUNCTION(Reliable, Server)
    void Server_ResetAllTweaks();

    UFUNCTION(Reliable, Server)
    void Server_SetTweakValue(int32 TweakIndex, int32 TweakCategoryIndex, float Value);

    UFUNCTION(Reliable, Server)
    void Server_SetTweakProfile(int32 ProfileIndex);

    int32 GetTweakProfileIndex() const { return TweakProfileIndex; }

    float GetTweakCurrentValue(int32 TweakIndex, int32 CategoryIndex);

    float* GetTweakCurrentValuePtr(int32 TweakIndex, int32 TweakCategoryIndex);

protected:

    void SetTweakCurrentValue(int32 TweakIndex, int32 TweakCategoryIndex, float Value);

    static void ApplyTweakOnActor(const AActor* Actor, const FCogAbilityTweak& Tweak, float Value, const FGameplayTag& SetByCallerMagnitudeTag);

    void ApplyAllTweaksOnAllActors();

    void ApplyAllTweaksOnActor(const AActor* Actor);

    void ApplyAllTweaksOnActor(int32 TweakCategoryIndex, const AActor* Actor);

    void FindActorsFromTweakCategory(int32 TweakCategoryIndex, TArray<AActor*>& Actors) const;

    int32 FindTweakCategoryFromActor(const AActor* Actor) const;

    static bool IsActorMatchingTweakCategory(const AActor* Actor, const UAbilitySystemComponent* ActorAbilitySystem, const FCogAbilityTweakCategory& TweakCategory);

    UPROPERTY(Replicated)
    int32 TweakProfileIndex = INDEX_NONE;

    UPROPERTY(Replicated)
    TArray<float> TweakCurrentValues;

    UPROPERTY(Config)
    TArray<FString> AppliedCheatsOnLocalPawn;

    UPROPERTY()
    TObjectPtr<APlayerController> OwnerPlayerController;

    FDelegateHandle OnAnyActorSpawnedHandle;

    UPROPERTY()
    TObjectPtr<const UCogAbilityDataAsset> AbilityAsset = nullptr;
};
