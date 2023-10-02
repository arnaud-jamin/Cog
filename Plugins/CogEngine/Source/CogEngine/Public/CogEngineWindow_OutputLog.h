#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "imgui.h"
#include "Misc/OutputDevice.h"
#include "CogEngineWindow_OutputLog.generated.h"

class UCogEngineWindow_OutputLog;

class UCogLogOutputDevice : public FOutputDevice
{
public:
    friend class UCogEngineWindow_OutputLog;

    UCogLogOutputDevice();
    ~UCogLogOutputDevice();

    virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

    UCogEngineWindow_OutputLog* OutputLog = nullptr;
};

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_OutputLog : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_OutputLog();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;

    void Clear();

    void AddLog(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category);

private:

    struct FLineInfo
    {
        int32 Start = 0;
        int32 End = 0;
        int32 Frame = 0;
        ELogVerbosity::Type Verbosity;
        FName Category;
    };

    void DrawRow(const char* BufferStart, const FLineInfo& Info, bool IsTableShown);

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

    ImGuiTextBuffer TextBuffer;

    ImGuiTextFilter Filter;

    TArray<FLineInfo> LineInfos;

    UCogLogOutputDevice OutputDevice;
};
