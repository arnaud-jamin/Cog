#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "CogEngineDataAsset.generated.h"

class FCogWindow;
//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogEngineCheat_ActiveState : uint8
{
    Inactive,
    Partial,
    Active,
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType, Blueprintable, Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories, Meta = (ShowWorldContextPin))
class COGENGINE_API UCogEngineCheat_Execution
    : public UObject
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    void Execute(const UObject* WorldContextObject, const AActor* Instigator, const TArray<AActor*>& Targets) const;

    UFUNCTION(BlueprintNativeEvent, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    ECogEngineCheat_ActiveState IsActiveOnTargets(const UObject* WorldContextObject, const TArray<AActor*>& Targets) const;

    virtual bool GetColor(const FCogWindow& InCallingWindow, FLinearColor& OutColor) const;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogEngineCheat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name;

    UPROPERTY(EditAnywhere, Instanced)
    TObjectPtr<UCogEngineCheat_Execution> Execution;

    UPROPERTY(EditAnywhere)
    FLinearColor Color = FLinearColor::White;

    mutable FLinearColor CustomColor = FLinearColor::White;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogEngineCheatCategory
{
    GENERATED_BODY()

    UPROPERTY(Category = "Cheats", EditAnywhere)
    FString Name;

    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogEngineCheat> PersistentCheats;

    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogEngineCheat> InstantCheats;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogEngineSpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> Class;

    UPROPERTY(EditAnywhere)
    TObjectPtr<const UObject> Asset = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogEngineSpawnGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name;

    UPROPERTY(EditAnywhere)
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere)
    TArray<FCogEngineSpawnEntry> Spawns;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGENGINE_API UCogEngineDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogEngineDataAsset() {}

    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogEngineCheatCategory> CheatCategories;

    UPROPERTY(Category = "Spawns", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogEngineSpawnGroup> SpawnGroups;

    UPROPERTY(Category = "Selection", EditAnywhere)
    TArray<TSubclassOf<AActor>> SelectionFilters = { ACharacter::StaticClass(), AActor::StaticClass(), AGameModeBase::StaticClass(), AGameStateBase::StaticClass() };
    
    UPROPERTY(Category = "Selection", EditAnywhere)
    TEnumAsByte<ETraceTypeQuery> SelectionTraceChannel = UEngineTypes::ConvertToTraceType(ECC_Pawn);
};
