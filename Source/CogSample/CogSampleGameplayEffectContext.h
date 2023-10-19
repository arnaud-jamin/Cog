#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CogSampleGameplayEffectContext.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCogSampleGameplayEffectContextFloatValue
{
    GENERATED_BODY()

    virtual ~FCogSampleGameplayEffectContextFloatValue()
    {
    }

    UPROPERTY(BlueprintReadWrite)
    FName Name;

    UPROPERTY(BlueprintReadWrite)
    float Value = 0.0f;

    virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

//--------------------------------------------------------------------------------------------------------------------------

template<>
struct TStructOpsTypeTraits<FCogSampleGameplayEffectContextFloatValue> : public TStructOpsTypeTraitsBase2<FCogSampleGameplayEffectContextFloatValue>
{
    enum
    {
        WithNetSerializer = true
    };
};

//--------------------------------------------------------------------------------------------------------------------------

USTRUCT()
struct FCogSampleGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

public:

    FCogSampleGameplayEffectContext()
        : FGameplayEffectContext()
    {
    }

    FCogSampleGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
        : FGameplayEffectContext(InInstigator, InEffectCauser)
    {
    }

    /** Returns the wrapped FGPCoreGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
    static FCogSampleGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

    virtual UScriptStruct* GetScriptStruct() const override;

    virtual FCogSampleGameplayEffectContext* Duplicate() const override;

    virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

    bool CanGameplayCueBePredicted();

    /* NOT REPLICATED - Is this effect context made for a local gameplay cue. Used to know if a remote gameplay cue
    * created by the local player should be filtered when a local gameplay cue is played instead.
    */
    bool bGameplayCueIsPredicted = false;

    /* REPLICATED */
    UPROPERTY()
    TArray<FCogSampleGameplayEffectContextFloatValue> FloatValues;
};

//--------------------------------------------------------------------------------------------------------------------------

template<>
struct TStructOpsTypeTraits<FCogSampleGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FCogSampleGameplayEffectContext>
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true
    };
};

//--------------------------------------------------------------------------------------------------------------------------

UCLASS(meta = (ScriptName = "CogSampleEffectContextLibrary"))
class UCogSampleEffectContextLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure)
    static bool EffectContextGetGameplayCueIsPredicted(FGameplayEffectContextHandle EffectContext);

    UFUNCTION(BlueprintCallable)
    static void EffectContextSetFloatValue(FGameplayEffectContextHandle EffectContext, FName Name, float Value);

    UFUNCTION(BlueprintPure)
    static void EffectContextGetFloatValue(FGameplayEffectContextHandle EffectContext, FName Name, float& Value, bool& Found);

    UFUNCTION(BlueprintPure)
    static void EffectContextGetAllFloatValues(FGameplayEffectContextHandle EffectContext, TArray<FCogSampleGameplayEffectContextFloatValue>& Entries);
};