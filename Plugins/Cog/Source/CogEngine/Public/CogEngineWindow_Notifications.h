#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogWidgets.h"
#include "imgui.h"
#include "Misc/OutputDevice.h"
#include "CogEngineWindow_Notifications.generated.h"

class UCogEngineConfig_Notifications;
class FCogEngineWindow_Notifications;

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
class COGENGINE_API FCogEngineWindow_Notifications : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

    void OnLogReceived(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity, const FName& InCategory);

    void Clear();
    void AddNotification(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity);

protected:

    struct FNotification
    {
        FString Id;
        FDateTime Time;
        ELogVerbosity::Type Verbosity;
        FString Message;
    };
    
    virtual void RenderHelp() override;

    virtual void RenderContent() override;
    
    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderNotifications();

    virtual void RenderSettings() override;

    ImGuiTextFilter Filter;

    FCogNotificationOutputDevice OutputDevice;

    TArray<FNotification> Notifications;
    
    static int32 NotificationsId;

    TWeakObjectPtr<UCogEngineConfig_Notifications> Config;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Notifications : public UCogCommonConfig
{
    GENERATED_BODY()

public:
    
    UPROPERTY(Config)
    bool DisableNotifications = false;

    UPROPERTY(Config)
    bool NotifyAllWarnings = false;

    UPROPERTY(Config)
    bool NotifyAllErrors = false;

    UPROPERTY(Config)
    bool NotifyConsoleCommands = true;
    
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
    FVector2f Alignment = FVector2f(1.0f, 1.0f);

    UPROPERTY(Config)
    FIntVector2 Padding = FIntVector2(10, 10);

    UPROPERTY(Config)
    bool UseFixedWidth = true;

    UPROPERTY(Config)
    int32 MaxHeight = 100;
    
    UPROPERTY(Config)
    int32 TextWrapping = 200;

    UPROPERTY(Config)
    int32 Rounding = 6;

    UPROPERTY(Config)
    bool ShowBorder = true;

    UPROPERTY(Config)
    float Duration = 5.0f;

    UPROPERTY(Config)
    float FadeOut = 0.5f;
    
    virtual void Reset() override
    {
        Super::Reset();

        TextDefaultColor        = FColor(200, 200, 200, 255);
        TextWarningColor        = FColor(255, 200,   0, 255); 
        TextErrorColor          = FColor(240,  77,  77, 255);
        BackgroundDefaultColor  = FColor( 15,  15,  15, 150);
        BackgroundWarningColor  = FColor( 23,   9,   0, 150); 
        BackgroundErrorColor    = FColor( 21,   0,   0, 150);
        BorderDefaultColor      = FColor(200, 200, 200, 100);
        BorderWarningColor      = FColor(255, 200,   0, 100); 
        BorderErrorColor        = FColor(240,  77,  77, 100);
        
        Alignment = { 1, 1 };
        Padding = { 10, 10 };
        UseFixedWidth = true;
        TextWrapping = 200;
        MaxHeight = 100;
        Rounding = 6;
        ShowBorder = true;
        Duration = 5.0f;
        FadeOut = 0.5f;
    }
};