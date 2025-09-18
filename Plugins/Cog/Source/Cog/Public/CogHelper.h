#pragma once

#include "AssetRegistry/AssetData.h"
#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "UObject/FieldIterator.h"

enum ECollisionChannel : int;
class UCollisionProfile;

class COG_API FCogHelper
{
public:

    static FString GetActorName(const AActor* Actor);

    static FString GetActorName(const AActor& Actor);

    static bool ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max);

    template<typename T>
    static const T* GetFirstAssetByClass();

    static const UObject* GetFirstAssetByClass(const TSubclassOf<UObject>& AssetClass);

    template<typename TCLass, typename TMember>
    static FProperty* FindProperty(TCLass* Instance,  TMember TCLass::*PointerToMember);

    static bool IsTraceChannelHidden(const UCollisionProfile& InCollisionProfile, ECollisionChannel InCollisionChannel);
};

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T* FCogHelper::GetFirstAssetByClass()
{
    return Cast<T>(GetFirstAssetByClass(T::StaticClass()));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TCLass, typename TMember>
FProperty* FCogHelper::FindProperty(TCLass* Instance,  TMember TCLass::*PointerToMember)
{
    for (TFieldIterator<FProperty> It(Instance->GetClass()); It; ++It)
    {
        FProperty* Property = *It;
        
        if (Property == nullptr)
        { continue; }

        const void* MemberAddress = &(Instance->*PointerToMember);
        const void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(Instance);

        if (MemberAddress == PropertyAddress)
        { return Property; }
    }

    return nullptr;
}