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

    static ACogAbilityReplicator* GetLocalReplicator(UWorld& World);

    static void GetRemoteReplicators(UWorld& World, TArray<ACogAbilityReplicator*>& Replicators);

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    void ApplyCheat(AActor* CheatInstigator, const TArray<AActor*>& Targets, const FCogAbilityCheat& Cheat);
    
    static bool IsCheatActive(const AActor* EffectTarget, const FCogAbilityCheat& Cheat);

    void GiveAbility(AActor* TargetActor, TSubclassOf<UGameplayAbility> AbilityClass) const;
    
    void RemoveAbility(AActor* TargetActor, const FGameplayAbilitySpecHandle& Handle) const;

    void ResetAllTweaks();

    void SetTweakValue(int32 TweakIndex, int32 CategoryIndex, float Value);

    void SetTweakProfile(int32 ProfileIndex);

    int32 GetTweakProfileIndex() const { return TweakProfileIndex; }

    float GetTweakCurrentValue(int32 TweakIndex, int32 CategoryIndex);

    float* GetTweakCurrentValuePtr(int32 TweakIndex, int32 CategoryIndex);

private:

    UFUNCTION()
    void OnAnyActorSpawned(AActor* Actor);

    UFUNCTION(Reliable, Server)
    void Server_ApplyCheat(const AActor* CheatInstigator, const TArray<AActor*>& TargetActors, const FCogAbilityCheat& Cheat) const;

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

    void SetTweakCurrentValue(int32 TweakIndex, int32 CategoryIndex, float Value);

    void ApplyAllTweaksOnAllActors();

    void ApplyTweakOnActor(AActor* Actor, const FCogAbilityTweak& Tweak, float Value, const FGameplayTag& SetByCallerMagnitudeTag);

    void ApplyAllTweaksOnActor(int32 TweakCategoryIndex, AActor* Actor);

    void FindActorsFromTweakCategory(int32 CategoryIndex, TArray<AActor*>& Actors);

    int32 FindTweakCategoryFromActor(AActor* Actor);

    static bool IsActorMatchingTweakCategory(const AActor* Actor, const UAbilitySystemComponent* ActorAbilitySystem, const FCogAbilityTweakCategory& TweakCategory);

    UPROPERTY(Replicated)
    int32 TweakProfileIndex = INDEX_NONE;

    UPROPERTY(Replicated)
    TArray<float> TweakCurrentValues;

    UPROPERTY(Config)
    TArray<FString> AppliedCheatsOnLocalPawn;

    TObjectPtr<APlayerController> OwnerPlayerController;

    FDelegateHandle OnAnyActorSpawnedHandle;

    UPROPERTY()
    TObjectPtr<const UCogAbilityDataAsset> AbilityAsset = nullptr;
};
