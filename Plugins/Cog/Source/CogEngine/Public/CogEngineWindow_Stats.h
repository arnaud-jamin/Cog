#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Stats.generated.h"

class UCogEngineWindowConfig_Stats;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Stats : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void Initialize() override;

    virtual void RenderHelp() override;

    virtual void RenderContextMenu() override;

    virtual void RenderContent() override;

    virtual void RenderMainMenuWidget() override;

    virtual void RenderMainMenuWidgetPacketLoss();

    virtual void RenderMainMenuWidgetPing();

    virtual void RenderMainMenuWidgetFrameRate();
    
    TWeakObjectPtr<UCogEngineWindowConfig_Stats> Config;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineWindowConfig_Stats : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    virtual void Reset() override
    {
        UCogCommonConfig::Reset();

        GoodColor = FColor(30, 255, 60, 255);
        MediumColor = FColor(255, 200,0, 255);
        BadColor = FColor(255, 80, 80, 255);
        
        FrameRates = { 0, 10, 20, 30, 60, 120 };
        Pings = { 0, 50, 100, 200, 500, 1000 };
        PacketLosses = { 0, 5, 10, 20, 30, 40, 50 };

        GoodFrameRate = 60;
        MediumFrameRate = 30;
        GoodPing = 100;
        MediumPing = 200;
        GoodPacketLoss = 5;
        MediumPacketLoss = 10;
    }

    ImVec4 GetFpsColor(float Value) const;

    ImVec4 GetPingColor(float Value) const;

    ImVec4 GetPacketLossColor(float Value) const;

    void RenderAllConfigs();
    
    void RenderColorConfig();
    
    void RenderFrameRateConfig();
    
    void RenderPingConfig();
    
    void RenderPacketLossConfig();

    UPROPERTY(Config)
    TArray<int> FrameRates;

    UPROPERTY(Config)
    int GoodFrameRate = 0;

    UPROPERTY(Config)
    int MediumFrameRate = 0;
    
    UPROPERTY(Config)
    TArray<int> Pings;

    UPROPERTY(Config)
    int GoodPing = 0;

    UPROPERTY(Config)
    int MediumPing = 0;

    UPROPERTY(Config)
    TArray<int> PacketLosses;

    UPROPERTY(Config)
    int GoodPacketLoss = 0;

    UPROPERTY(Config)
    int MediumPacketLoss = 0;
    
    UPROPERTY(Config)
    FColor GoodColor = FColor();

    UPROPERTY(Config)
    FColor MediumColor = FColor();

    UPROPERTY(Config)
    FColor BadColor = FColor();
};
