#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogEngineWindow_Cheats.generated.h"

class ACogEngineReplicator;
class AActor;
class UCogEngineConfig_Cheats;
class UCogEngineDataAsset;
struct FCogEngineCheat;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Cheats : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void GameTick(float DeltaTime) override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void TryReapplyCheats();

    virtual bool AddCheat(ACogEngineReplicator& Replicator, const int32 Index, AActor* ControlledActor, AActor* TargetActor, const FCogEngineCheat& CheatEffect, bool IsPersistent);

    virtual void RequestCheat(ACogEngineReplicator& Replicator, AActor* ControlledActor, AActor* SelectedActor, const FCogEngineCheat& Cheat, bool ApplyToEnemies, bool ApplyToAllies, bool ApplyToControlled);

    virtual const FCogEngineCheat* FindCheatByName(const FString& CheatName, const bool OnlyPersistentCheats);

    static bool DrawTable();

    void UpdateCheatColor(const FCogEngineCheat& Cheat) const;

    TObjectPtr<const UCogEngineDataAsset> Asset = nullptr;

    TObjectPtr<UCogEngineConfig_Cheats> Config = nullptr;

    bool bHasReappliedCheats = false;

    ImGuiTextFilter Filter;

    TArray<FCogEngineCheat> AllCheats;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Cheats : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bGroupByCategories = true;

    UPROPERTY(Config)
    bool bReapplyCheatsBetweenPlays = true;

    UPROPERTY(Config)
    bool bReapplyCheatsBetweenLaunches = true;

    UPROPERTY(Config)
    bool bUseTwoColumns = true;

    UPROPERTY(Config)
    TArray<FString> AppliedCheats;

    UPROPERTY(Config)
    TSet<FString> SelectedCategories;

    virtual void Reset() override
    {
        Super::Reset();

        bReapplyCheatsBetweenPlays = true;
        bReapplyCheatsBetweenLaunches = true;
        AppliedCheats.Empty();
        SelectedCategories.Empty();
    }
};
