#pragma once

#include "CoreMinimal.h"
#include "CogSampleProgressionLevelInterface.h"
#include "CogSampleTeamInterface.h"
#include "CogSampleSpawnableInterface.h"
#include "GameFramework/Actor.h"
#include "CogSampleBasicActor.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(config=Game)
class ACogSampleBasicActor : public AActor
    , public ICogSampleProgressionLevelInterface
    , public ICogSampleSpawnableInterface
    , public ICogSampleTeamInterface
{
	GENERATED_BODY()

public:

    ACogSampleBasicActor(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay();
    
    UFUNCTION(BlueprintPure)
    virtual int32 GetTeam() const override { return Team; }

    UFUNCTION(BlueprintCallable)
    virtual void SetTeam(int32 Value) override;

    UFUNCTION(BlueprintPure)
    virtual int32 GetProgressionLevel() const override { return ProgressionLevel; }

    UFUNCTION(BlueprintCallable)
    virtual void SetProgressionLevel(int32 Value) override;

    UFUNCTION(BlueprintPure)
    virtual AActor* GetCreator() const override { return Creator.Get(); }

    UFUNCTION(BlueprintCallable)
    virtual void SetCreator(AActor* Value) override;

protected:
   
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess, ExposeOnSpawn))
    int32 Team = 0;
    
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess, ExposeOnSpawn))
    int32 ProgressionLevel = 0;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess, ExposeOnSpawn))
    TWeakObjectPtr<AActor> Creator = nullptr;
};

