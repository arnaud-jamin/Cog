#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Cheats.generated.h"

class AActor;
class UCogAbilityDataAsset;
struct FCogAbilityCheat;

UCLASS(Config = Cog)
class COGABILITY_API UCogAbilityWindow_Cheats : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAbilityWindow_Cheats();

    const UCogAbilityDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogAbilityDataAsset* Value);

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

private:
    
    bool AddCheat(AActor* ControlledActor, AActor* TargetActor, const FCogAbilityCheat& CheatEffect, bool IsPersistent);

    void RequestCheat(AActor* ControlledActor, AActor* TargetActor, const FCogAbilityCheat& CheatEffect);

    UPROPERTY(Config)
    bool bReapplyCheatsBetweenPlays = true;

    UPROPERTY(Config)
    bool bReapplyCheatsBetweenLaunches = true;

    UPROPERTY(Config)
    TArray<FString> AppliedCheats;

    UPROPERTY()
    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
};
