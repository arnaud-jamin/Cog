#include "CogSampleGameplayEffectContext.h"

#include "AbilitySystemComponent.h"

//--------------------------------------------------------------------------------------------------------------------------
bool FCogSampleGameplayEffectContextFloatValue::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
    enum RepFlag
    {
        REP_Name = 0,
        REP_Value,
        REP_MAX
    };

    uint16 RepBits = 0;
    if (Ar.IsSaving())
    {
        if (Name.IsValid())
        {
            RepBits |= (1 << REP_Name);
        }

        if (Value != 0.f)
        {
            RepBits |= (1 << REP_Value);
        }
    }

    Ar.SerializeBits(&RepBits, REP_MAX);


    if (RepBits & (1 << REP_Name))
    {
        Ar << Name;
    }

    if (RepBits & (1 << REP_Value))
    {
        Ar << Value;
    }

    bOutSuccess = true;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------

FCogSampleGameplayEffectContext* FCogSampleGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
    FGameplayEffectContext* BaseEffectContext = Handle.Get();
    if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FCogSampleGameplayEffectContext::StaticStruct()))
    {
        return (FCogSampleGameplayEffectContext*)BaseEffectContext;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------

bool FCogSampleGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    enum RepFlag
    {
        Rep_Instigator = 0,
        Rep_EffectCauser,
        Rep_AbilityCDO,
        Rep_SourceObject,
        Rep_Actors,
        Rep_HitResult,
        Rep_HasWorldOrigin,

        Rep_FloatValues,
        Rep_MAX
    };

    uint32 RepBits = 0;
    if (Ar.IsSaving())
    {
        if (Instigator.IsValid())
        {
            RepBits |= 1 << Rep_Instigator;
        }
        if (EffectCauser.IsValid())
        {
            RepBits |= 1 << Rep_EffectCauser;
        }
        if (AbilityCDO.IsValid())
        {
            RepBits |= 1 << Rep_AbilityCDO;
        }
        if (bReplicateSourceObject && SourceObject.IsValid())
        {
            RepBits |= 1 << Rep_SourceObject;
        }
        if (Actors.Num() > 0)
        {
            RepBits |= 1 << Rep_Actors;
        }
        if (HitResult.IsValid())
        {
            RepBits |= 1 << Rep_HitResult;
        }
        if (bHasWorldOrigin)
        {
            RepBits |= 1 << Rep_HasWorldOrigin;
        }

        //------------------------------
        // Our custom fields
        //------------------------------
        if (FloatValues.Num() > 0)
        {
            RepBits |= 1 << Rep_FloatValues;
        }
        //------------------------------
    }

    Ar.SerializeBits(&RepBits, Rep_MAX);

    if (RepBits & (1 << Rep_Instigator))
    {
        Ar << Instigator;
    }
    if (RepBits & (1 << Rep_EffectCauser))
    {
        Ar << EffectCauser;
    }
    if (RepBits & (1 << Rep_AbilityCDO))
    {
        Ar << AbilityCDO;
    }
    if (RepBits & (1 << Rep_SourceObject))
    {
        Ar << SourceObject;
    }
    if (RepBits & (1 << Rep_Actors))
    {
        SafeNetSerializeTArray_Default<31>(Ar, Actors);
    }
    if (RepBits & (1 << Rep_HitResult))
    {
        if (Ar.IsLoading())
        {
            if (!HitResult.IsValid())
            {
                HitResult = TSharedPtr<FHitResult>(new FHitResult());
            }
        }
        HitResult->NetSerialize(Ar, Map, bOutSuccess);
    }
    if (RepBits & (1 << Rep_HasWorldOrigin))
    {
        Ar << WorldOrigin;
        bHasWorldOrigin = true;
    }
    else
    {
        bHasWorldOrigin = false;
    }

    //------------------------------
    // Our custom fields
    //------------------------------
    if (RepBits & (1 << Rep_FloatValues))
    {
        SafeNetSerializeTArray_WithNetSerialize<31>(Ar, FloatValues, Map);
    }

    if (Ar.IsLoading())
    {
        AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
    }

    bOutSuccess = true;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------

UScriptStruct* FCogSampleGameplayEffectContext::GetScriptStruct() const
{
    return FCogSampleGameplayEffectContext::StaticStruct();
}

//--------------------------------------------------------------------------------------------------------------------------

FCogSampleGameplayEffectContext* FCogSampleGameplayEffectContext::Duplicate() const
{
    FCogSampleGameplayEffectContext* newContext = new FCogSampleGameplayEffectContext();
    *newContext = *this;
    newContext->AddActors(Actors);
    if (GetHitResult())
    {
        // Does a deep copy of the hit result
        newContext->AddHitResult(*GetHitResult(), true);
    }
    return newContext;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleEffectContextLibrary::EffectContextSetFloatValue(FGameplayEffectContextHandle EffectContext, FName Name, float Value)
{
    if (FCogSampleGameplayEffectContext* TypedEffectContext = FCogSampleGameplayEffectContext::ExtractEffectContext(EffectContext))
    {
        FCogSampleGameplayEffectContextFloatValue* Result = TypedEffectContext->FloatValues.FindByPredicate([&Name](const FCogSampleGameplayEffectContextFloatValue& Entry)
            {
                return Entry.Name == Name;
            });

        if (Result != nullptr)
        {
            Result->Value = Value;
        }
        else
        {
            FCogSampleGameplayEffectContextFloatValue Entry;
            Entry.Name = Name;
            Entry.Value = Value;
            TypedEffectContext->FloatValues.Emplace(Entry);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleEffectContextLibrary::EffectContextGetFloatValue(FGameplayEffectContextHandle EffectContext, FName Name, float& Value, bool& Found)
{
    Found = false;

    if (FCogSampleGameplayEffectContext* TypedEffectContext = FCogSampleGameplayEffectContext::ExtractEffectContext(EffectContext))
    {
        const FCogSampleGameplayEffectContextFloatValue* Result = TypedEffectContext->FloatValues.FindByPredicate([&Name](const FCogSampleGameplayEffectContextFloatValue& Entry)
            {
                return Entry.Name == Name;
            });

        if (Result != nullptr)
        {
            Value = Result->Value;
            Found = true;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleEffectContextLibrary::EffectContextGetAllFloatValues(FGameplayEffectContextHandle EffectContext, TArray<FCogSampleGameplayEffectContextFloatValue>& Entries)
{
    if (FCogSampleGameplayEffectContext* TypedEffectContext = FCogSampleGameplayEffectContext::ExtractEffectContext(EffectContext))
    {
        Entries = TypedEffectContext->FloatValues;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleEffectContextLibrary::EffectContextGetGameplayCueIsPredicted(FGameplayEffectContextHandle EffectContext)
{
    if (FCogSampleGameplayEffectContext* TypedEffectContext = FCogSampleGameplayEffectContext::ExtractEffectContext(EffectContext))
    {
        return TypedEffectContext->bGameplayCueIsPredicted;
    }

    return false;
}
