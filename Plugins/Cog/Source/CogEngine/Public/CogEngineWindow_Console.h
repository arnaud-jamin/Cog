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

    static int OnTextInputCallbackStub(ImGuiInputTextCallbackData* InData);

    static FString GetConsoleCommandHelp(const FString& InCommandName);

    void RenderConsoleTextInput();

    void RenderCommand(const FString& CommandName, int32 Index);
    
    void RefreshVisibleCommands();

    int OnTextInputCallback(ImGuiInputTextCallbackData* InData);

    void ExecuteCommand(const FString& InCommand);

    int32 SelectedCommandIndex = 0;
    
    FString SelectedCommand;

    TArray<FString> VisibleCommands;
    
    TArray<FString> VisibleHistory;

    FString CurrentUserInput;
    
    bool bScroll = false;

    bool bRequestTextInputFocus = false;
    
    bool bIsWindowFocused = false;
    
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
    bool ShowConsoleInputInMenuBar = false;

    UPROPERTY(Config)
    int32 NumHistoryCommands = 10;

    UPROPERTY(Config)
    int32 CompletionMinimumCharacters = 0;

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
        NumHistoryCommands = 10;
        CompletionMinimumCharacters = 0;
        ShowConsoleInputInMenuBar = false;
        HistoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);
    }
};