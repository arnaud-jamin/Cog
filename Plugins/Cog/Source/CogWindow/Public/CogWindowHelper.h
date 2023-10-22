#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

class COGWINDOW_API FCogWindowHelper
{
public:

    template<typename T>
    static T* GetFirstAssetByClass()
    {
        IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

        TArray<FAssetData> Assets;
        AssetRegistry.GetAssetsByClass(T::StaticClass()->GetClassPathName(), Assets, true);
        if (Assets.Num() == 0)
        {
            return nullptr;
        }

        UObject* Asset = Assets[0].GetAsset();
        T* CastedAsset = Cast<T>(Asset);
        return CastedAsset;
    }
};
