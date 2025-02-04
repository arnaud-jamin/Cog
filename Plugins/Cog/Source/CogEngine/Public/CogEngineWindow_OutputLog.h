#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "imgui.h"
#include "Misc/OutputDevice.h"
#include "CogEngineWindow_OutputLog.generated.h"

class UCogEngineConfig_OutputLog;

//--------------------------------------------------------------------------------------------------------------------------
class FCogLogOutputDevice : public FOutputDevice
{
public:
    friend class FCogEngineWindow_OutputLog;

    FCogLogOutputDevice();
    virtual ~FCogLogOutputDevice() override;

    virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

    FCogEngineWindow_OutputLog* OutputLog = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_OutputLog : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

    void AddLog(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category);

    void Clear();

    void Copy() const;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

private:

    struct FLineInfo
    {
        int32 Start = 0;
        int32 End = 0;
        int32 Frame = 0;
        ELogVerbosity::Type Verbosity;
        FName Category;
    };

    void DrawRow(const char* BufferStart, const FLineInfo& Info, bool IsTableShown) const;

    ImGuiTextBuffer TextBuffer;

    ImGuiTextFilter Filter;

    TArray<FLineInfo> LineInfos;

    FCogLogOutputDevice OutputDevice;

    TWeakObjectPtr<UCogEngineConfig_OutputLog> Config;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_OutputLog : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool AutoScroll = true;

    UPROPERTY(Config)
    bool ShowFrame = true;

    UPROPERTY(Config)
    bool ShowCategory = true;

    UPROPERTY(Config)
    bool ShowVerbosity = false;

    UPROPERTY(Config)
    bool ShowAsTable = false;

    UPROPERTY(Config)
    int32 VerbosityFilter = ELogVerbosity::VeryVerbose;

    UPROPERTY(Config)
    FColor DefaultColor =  FColor::White;
    
    UPROPERTY(Config)
    FColor WarningColor = FColor::White;

    UPROPERTY(Config)
    FColor ErrorColor = FColor::White;

    virtual void Reset() override
    {
        Super::Reset();

        AutoScroll = true;
        ShowFrame = true;
        ShowCategory = true;
        ShowVerbosity = false;
        ShowAsTable = false;
        VerbosityFilter = ELogVerbosity::VeryVerbose;
        DefaultColor =  FColor(200, 200, 200, 255);
        WarningColor = FColor(255, 200, 0, 255); 
        ErrorColor = FColor(255, 0, 0, 255); 
    }
};