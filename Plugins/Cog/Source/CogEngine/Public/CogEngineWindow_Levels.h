#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetData.h"
#include "CogWindow.h"
#include "CogEngineWindow_Levels.generated.h"

class UCogEngineWindowConfig_LevelLoader;

class COGENGINE_API FCogEngineWindow_Levels : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderMenu();

    virtual void RenderLevel(int32 InIndex, const FAssetData& InAsset);

    virtual void RenderLevelContextMenu(int Index, const FAssetData& Asset);

    virtual void GetAllLevels(TArray<FAssetData>& OutLevels);
    void RefreshSorting();

    virtual void LoadLevel(const FAssetData& InAsset);


private:

    TArray<FAssetData> Levels;
    
    TArray<FAssetData> UnsortedLevels;

    bool HasGatheredLevels = false; 

    int32 SelectedIndex = -1;

    TObjectPtr<UCogEngineWindowConfig_LevelLoader> Config = nullptr;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineWindowConfig_LevelLoader : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    virtual void Reset() override
    {
        UCogCommonConfig::Reset();

        SortByName = true;
        ShowPath = false;
    }
    
    UPROPERTY(Config)
    bool SortByName = true;

    UPROPERTY(Config)
    bool ShowPath = false;
};
