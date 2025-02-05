#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "imgui.h"
#include "Misc/OutputDevice.h"
#include "CogEngineWindow_Notifications.generated.h"

class UCogEngineConfig_Notifications;

//--------------------------------------------------------------------------------------------------------------------------
class FCogNotificationOutputDevice : public FOutputDevice
{
public:
    friend class FCogEngineWindow_Notifications;

    FCogNotificationOutputDevice();
    virtual ~FCogNotificationOutputDevice() override;

    virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

    FCogEngineWindow_Notifications* Notifications = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngineNotificationLocation : uint8
{
    TopLeft     = 0,
    TopRight    = 1,
    BottomLeft  = 2,
    BottomRight = 3,
};

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Notifications : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

    void AddLog(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity, const FName& InCategory);

    void Clear();

protected:

    struct FNotification
    {
        int32 LineStart = 0;
        int32 LineEnd = 0;
        uint64 Frame = 0;
        FDateTime Time;
        ELogVerbosity::Type Verbosity;
        FName Category;
        FString Message; 
    };
    
    virtual void RenderHelp() override;

    virtual void RenderContent() override;
    
    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderNotifications();

    virtual void DrawRow(const FNotification& InNotification, bool InShowAsTableRow) const;

    ImGuiTextFilter Filter;

    FCogNotificationOutputDevice OutputDevice;

    TArray<FNotification> Notifications;

    TWeakObjectPtr<UCogEngineConfig_Notifications> Config;

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Notifications : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool AutoScroll = true;

    UPROPERTY(Config)
    bool ShowVerbosity = false;

    UPROPERTY(Config)
    int32 VerbosityFilter = ELogVerbosity::VeryVerbose;

    UPROPERTY(Config)
    bool ShowAsTable = false;

    UPROPERTY(Config)
    FColor BackgroundDefaultColor =  FColor::White;
    
    UPROPERTY(Config)
    FColor BackgroundWarningColor = FColor::White;

    UPROPERTY(Config)
    FColor BackgroundErrorColor = FColor::White;

    UPROPERTY(Config)
    FColor BorderDefaultColor =  FColor::White;
    
    UPROPERTY(Config)
    FColor BorderWarningColor = FColor::White;

    UPROPERTY(Config)
    FColor BorderErrorColor = FColor::White;
    
    UPROPERTY(Config)
    FColor TextDefaultColor =  FColor::White;
    
    UPROPERTY(Config)
    FColor TextWarningColor = FColor::White;

    UPROPERTY(Config)
    FColor TextErrorColor = FColor::White;

    UPROPERTY(Config)
    ECogEngineNotificationLocation Location = ECogEngineNotificationLocation::BottomRight;

    UPROPERTY(Config)
    int Padding = 10;

    UPROPERTY(Config)
    int32 TextWrapping = 30;

    UPROPERTY(Config)
    int32 WindowRounding = 6;

    UPROPERTY(Config)
    bool WindowBorder = false;

    UPROPERTY(Config)
    float VisibleDuration = 3.0f;

    UPROPERTY(Config)
    float FadeDuration = 0.5f;
    
    virtual void Reset() override
    {
        Super::Reset();

        AutoScroll = true;
        ShowVerbosity = false;
        VerbosityFilter = ELogVerbosity::VeryVerbose;
        ShowAsTable = false;
        TextDefaultColor        =  FColor(200, 200, 200, 255);
        TextWarningColor        = FColor(255, 200, 0, 255); 
        TextErrorColor          = FColor(255, 0, 0, 255);
        BackgroundDefaultColor  =  FColor(0, 0, 0, 170);
        BackgroundWarningColor  = FColor(50, 20, 0, 170); 
        BackgroundErrorColor    = FColor(50, 0, 0, 170);
        BorderDefaultColor      =  FColor(200, 200, 200, 100);
        BorderWarningColor      = FColor(255, 200, 0, 100); 
        BorderErrorColor        = FColor(255, 0, 0, 100);
        
        Location = ECogEngineNotificationLocation::BottomRight;
        Padding = 10;
        TextWrapping = 40;
        WindowRounding = 6;
        WindowBorder = true;
    }
};