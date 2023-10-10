#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogAbilityReplicator.generated.h"

class AActor;
class UAbilitySystemComponent;
class UCogAbilityDataAsset_Tweaks;
struct FCogAbilityCheat;
struct FCogAbilityTweak;
struct FGameplayTag;

UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient, Config = Cog)
class COGABILITY_API ACogAbilityReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:

    static void Create(APlayerController* Controller);

    virtual void BeginPlay() override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    void ApplyCheat(AActor* CheatInstigator, const TArray<AActor*>& Targets, const FCogAbilityCheat& Cheat);
    
    static bool IsCheatActive(const AActor* EffectTarget, const FCogAbilityCheat& Cheat);

    void GiveAbility(AActor* TargetActor, TSubclassOf<UGameplayAbility> AbilityClass) const;
    
    void RemoveAbility(AActor* TargetActor, const FGameplayAbilitySpecHandle& Handle) const;

    void ResetAllTweaks();

    void SetTweakValue(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakIndex, int32 CategoryIndex, float Value);

    void SetTweakProfile(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 ProfileIndex);

    int32 GetTweakProfileIndex() const { return TweakProfileIndex; }

    float GetTweakCurrentValue(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakIndex, int32 CategoryIndex);

    float* GetTweakCurrentValuePtr(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakIndex, int32 CategoryIndex);

private:

    UFUNCTION(Reliable, Server)
    void Server_ApplyCheat(const AActor* CheatInstigator, const TArray<AActor*>& TargetActors, const FCogAbilityCheat& Cheat) const;

    UFUNCTION(Reliable, Server)
    void Server_GiveAbility(AActor* TargetActor, TSubclassOf<UGameplayAbility> AbilityClass) const;

    UFUNCTION(Reliable, Server)
    void Server_RemoveAbility(AActor* TargetActor, const FGameplayAbilitySpecHandle& Handle) const;

    UFUNCTION(Reliable, Server)
    void Server_ResetAllTweaks();

    UFUNCTION(Reliable, Server)
    void Server_SetTweakValue(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakIndex, int32 TweakCategoryIndex, float Value);

    UFUNCTION(Reliable, Server)
    void Server_SetTweakProfile(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 ProfileIndex);

    void SetTweakCurrentValue(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakIndex, int32 CategoryIndex, float Value);
    void ApplyTweakOnActor(AActor* Actor, const FCogAbilityTweak& Tweak, float Value, const FGameplayTag& SetByCallerMagnitudeTag);
    void ApplyAllTweaksOnActor(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 TweakCategoryIndex, AActor* Actor);
    void GetActorsFromTweakCategory(const UCogAbilityDataAsset_Tweaks* TweaksAsset, int32 CategoryIndex, TArray<AActor*>& Actors);

    TObjectPtr<APlayerController> OwnerPlayerController;

    UPROPERTY(Replicated)
    int32 TweakProfileIndex = INDEX_NONE;

    UPROPERTY(Replicated)
    TArray<float> TweakCurrentValues;

    UPROPERTY(Config)
    TArray<FString> AppliedCheatsOnLocalPawn;
};
