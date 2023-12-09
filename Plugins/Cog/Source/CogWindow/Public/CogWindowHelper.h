#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetData.h"
#include "Templates/SubclassOf.h"


class COGWINDOW_API FCogWindowHelper
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    template<typename T>
    static const T* GetFirstAssetByClass()
    {
        return Cast<T>(GetFirstAssetByClass(T::StaticClass()));
    }

    //----------------------------------------------------------------------------------------------------------------------
    static const UObject* GetFirstAssetByClass(const TSubclassOf<UObject> AssetClass)
    {
        IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

        TArray<FAssetData> Assets;
        AssetRegistry.GetAssetsByClass(AssetClass->GetClassPathName(), Assets, true);
        if (Assets.Num() == 0)
        {
            return nullptr;
        }

        UObject* Asset = Assets[0].GetAsset();
        return Asset;
    }
};
