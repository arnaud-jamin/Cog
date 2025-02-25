#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWidgets.h"
#include "CogEngineWindow_BuildInfo.generated.h"

class UCogEngineConfig_BuildInfo;

class COGENGINE_API FCogEngineWindow_BuildInfo : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;
    
    virtual void RenderHelp() override;
    
    virtual void RenderTick(float DeltaTime) override;
    
    virtual void RenderContent() override;

protected:

    void BuildText();

    TWeakObjectPtr<UCogEngineConfig_BuildInfo> Config;

    FString Text;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_BuildInfo : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool ShowInEditor = false;

    UPROPERTY(Config)
    bool ShowInPackage = true;
        
    UPROPERTY(Config)
    bool ShowBranchName = false;

    UPROPERTY(Config)
    bool ShowBuildDate = true;
    
    UPROPERTY(Config)
    bool ShowCurrentChangelist = true;

    UPROPERTY(Config)
    bool ShowCompatibleChangelist = false;

    UPROPERTY(Config)
    bool ShowBuildConfiguration = true;

    UPROPERTY(Config)
    bool ShowBuildUser = false;

    UPROPERTY(Config)
    bool ShowBuildMachine = false;

    UPROPERTY(Config)
    bool ShowBuildTargetType = true;

    UPROPERTY(Config)
    bool ShowInForeground = true;
    
    UPROPERTY(Config)
    FVector2f Alignment =  { 0, 1 }; 

    UPROPERTY(Config)
    FIntVector2 Padding = { 10, 10 };

    UPROPERTY(Config)
    int32 Rounding = 6;

    UPROPERTY(Config)
    FString Separator = "|";
    
    UPROPERTY(Config)
    FColor BackgroundColor = FColor(0, 0, 0, 80);

    UPROPERTY(Config)
    FColor BorderColor = FColor(255, 255, 255, 50);

    UPROPERTY(Config)
    FColor TextColor = FColor(255, 255, 255, 100);
        
    virtual void Reset() override
    {
        Super::Reset();
        
        ShowInEditor = false;
        ShowInPackage = true;
        ShowInForeground = true;
        ShowBranchName = false;
        ShowBuildDate = true;
        ShowCurrentChangelist = true;
        ShowCompatibleChangelist = false;
        ShowBuildConfiguration = true;
        ShowBuildUser = false;
        ShowBuildMachine = false;
        ShowBuildTargetType = true;
        Alignment =  { 0, 1 }; 
        Padding = { 10, 10 };
        Rounding = 6;
        Separator = " | ";
        BackgroundColor = FColor(0, 0, 0, 80);
        BorderColor = FColor(255, 255, 255, 50);
        TextColor = FColor(255, 255, 255, 100);
    }
};