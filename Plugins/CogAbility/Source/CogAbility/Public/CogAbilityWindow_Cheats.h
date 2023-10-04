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

    TWeakObjectPtr<UCogAbilityDataAsset_Cheats> CheatsAsset;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

private:
    
    void AddCheat(AActor* InstigatorActor, AActor* TargetActor, const FCogAbilityCheat& CheatEffect, bool IsPersistent);

    void RequestCheat(AActor* InstigatorActor, AActor* TargetActor, const FCogAbilityCheat& CheatEffect);

    UPROPERTY(Config)
    TArray<FString> AppliedCheats;
};
