#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Cheats.generated.h"

class AActor;
class UCogAbilityDataAsset_Cheats;
struct FCogAbilityCheat;

UCLASS(Config = Cog)
class COGABILITY_API UCogAbilityWindow_Cheats : public UCogWindow
{
    GENERATED_BODY()

public:

    const UCogAbilityDataAsset_Cheats* GetCheatsAsset() const { return CheatsAsset.Get(); }

    void SetCheatsAsset(const UCogAbilityDataAsset_Cheats* Value);

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

private:
    
    void AddCheat(AActor* ControlledActor, AActor* TargetActor, const FCogAbilityCheat& CheatEffect, bool IsPersistent);

    void RequestCheat(AActor* ControlledActor, AActor* TargetActor, const FCogAbilityCheat& CheatEffect);

    UPROPERTY(Config)
    TArray<FString> AppliedCheats;

    TWeakObjectPtr<const UCogAbilityDataAsset_Cheats> CheatsAsset;
};
