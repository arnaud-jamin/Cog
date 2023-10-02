#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Damages.generated.h"

struct FCogInterfacesDamageParams;

//--------------------------------------------------------------------------------------------------------------------------
class FCogDamageInstance
{
public:
    void Restart();
    void AddDamage(const float Damage);
    void UpdateDamagePerSecond(const float Duration);

    float DamageLast = 0.0f;
    float DamageMin = 0.0f;
    float DamageMax = 0.0f;
    float DamagePerFrame = 0.0f;
    float DamagePerSecond = 0.0f;
    float DamageTotal = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
class FCogDamageStats
{
public:
    void AddDamage(const float Damage, const float UnmitigatedDamage, const bool bIsCrit);
    void Tick(const float DeltaSeconds);
    void Restart();

    int Count = 0;
    int Crits = 0;
    bool IsInProgress = false;
    float TotalCritChances = 0.0f;
    float Timer = 0.0f;
    float MaxDuration = 0.0f;
    float RestartTimer = 0.0f;

    FCogDamageInstance Mitigated;
    FCogDamageInstance Unmitigated;

    static float MaxDurationSetting;
    static float RestartDelaySetting;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS()
class COGABILITY_API UCogAbilityWindow_Damages : public UCogWindow
{
    GENERATED_BODY()

public:

protected:
    
    virtual void RenderContent() override;
    
    virtual void GameTick(float DeltaSeconds) override;

    virtual void OnSelectionChanged(AActor* OldSelection, AActor* NewSelection) override;

private:

    UFUNCTION()
    void OnDamageEvent(const FCogInterfacesDamageParams& Params);

    FCogDamageStats DamageDealtStats;
    FCogDamageStats DamageReceivedStats;
    FDelegateHandle OnDamageEventDelegate;
};
