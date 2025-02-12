#pragma once

#include "AssetRegistry/AssetData.h"
#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"


class COG_API FCogHelper
{
public:

    static FString GetActorName(const AActor* Actor);

    static FString GetActorName(const AActor& Actor);

    static bool ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max);

    template<typename T>
    static const T* GetFirstAssetByClass();

    static const UObject* GetFirstAssetByClass(const TSubclassOf<UObject>& AssetClass);
};

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T* FCogHelper::GetFirstAssetByClass()
{
    return Cast<T>(GetFirstAssetByClass(T::StaticClass()));
}
