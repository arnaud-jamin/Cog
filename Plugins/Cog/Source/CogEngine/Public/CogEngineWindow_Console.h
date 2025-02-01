#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Console.generated.h"

class UCogEngineConfig_Console;
class IConsoleObject;

class COGENGINE_API FCogEngineWindow_Console : public FCogWindow
{
    typedef FCogWindow Super;

public:
    
protected:

    virtual void RenderHelp() override;

    virtual void Initialize() override;

    virtual void RenderMainMenuWidget() override;
    
    virtual void RenderContent() override;

private:


    static FString GetConsoleCommandHelp(const FString& InCommandName);

    void RenderMenu();

    void RenderInput();

    int OnTextInputCallback(ImGuiInputTextCallbackData* InData);

    static int OnTextInputCallbackStub(ImGuiInputTextCallbackData* InData);

    void RenderCommandList();

    void RenderCommand(const FString& CommandName, int32 Index);

    void RefreshCommandList();

    void RenderCommandHelp();

    void ExecuteCommand(const FString& InCommand);

    int32 SelectedCommandIndex = -1;
    
    TArray<FString> CommandList;
    
    int32 NumHistoryCommands = 0;
    
    FString CurrentUserInput;
    
    bool bScroll = false;

    bool bRequestInputFocus = false;
    
    bool bIsWindowFocused = false;
    
    bool bPopupCommandListOnWidgetMode = false;

    ImGuiID InputIdOnWidgetMode = 0;

    bool bIsRenderingWidget = false;
    
    bool bSetBufferToSelectedCommand = false;
    
    TWeakObjectPtr<UCogEngineConfig_Console> Config;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Console : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool SortCommands = false;

    UPROPERTY(Config)
    bool DockInputInMenuBar = false;

    UPROPERTY(Config)
    bool FocusWidgetWhenAppearing = true;

    UPROPERTY(Config)
    int32 WidgetWidth = 200;
    
    UPROPERTY(Config)
    bool UseClipper = false;
    
    UPROPERTY(Config)
    int32 NumHistoryCommands = 10;

    UPROPERTY(Config)
    int32 CompletionMinimumCharacters = 1;
    
    UPROPERTY(Config)
    FVector4f HistoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);

    
    UCogEngineConfig_Console()
    {
        Reset();
    }

    virtual void Reset() override
    {
        Super::Reset();

        SortCommands = false;
        DockInputInMenuBar = false;
        FocusWidgetWhenAppearing = true;
        UseClipper = false;
        NumHistoryCommands = 10;
        CompletionMinimumCharacters = 1;
        HistoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);
    }
};