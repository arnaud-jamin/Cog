#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Console.generated.h"

class UCogEngineConfig_Console;
class IConsoleObject;

class COGENGINE_API FCogEngineWindow_Console : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void RenderHelp() override;

    virtual void Initialize() override;

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;

    virtual void PostBegin() override;

    virtual void RenderMainMenuWidget() override;
    
    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaTime) override;

private:

    static IConsoleObject* GetCommandObjectFromCommandLine(const FString& InCommandLine);

    static FString GetConsoleCommandHelp(const FString& InCommandLine);

    static int OnTextInputCallbackStub(ImGuiInputTextCallbackData* InData);

    void RenderMenu();

    void RenderInput();
    
    void SelectNextCommand();
    
    void SelectPreviousCommand();

    int OnTextInputCallback(ImGuiInputTextCallbackData* InData);

    void RenderCommandList();

    void RenderCommand(const FString& CommandName, int32 Index, float RegionMinY, float RegionMaxY);

    void RefreshCommandList();
    
    void ActivateInputText() const;

    void ExecuteCommand(const FString& InCommand);

    int32 SelectedCommandIndex = INDEX_NONE;
    
    TArray<FString> CommandList;
    
    int32 NumHistoryCommands = 0;
    
    FString CurrentUserInput;
    
    bool bScroll = false;

    bool bIsWindowFocused = false;
    
    bool bSetBufferToSelectedCommand = false;

    bool bIsWidgetMode = false;

    ImGuiID InputTextId = 0;

    bool WidgetMode_OpenCommandList = false;

    ImVec2 WidgetMode_CommandListPosition = ImVec2(0, 0);

    bool WidgetMode_IsTextInputActive = false;

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
    bool ShowHelp = true;
    
    UPROPERTY(Config)
    int32 NumHistoryCommands = 10;

    UPROPERTY(Config)
    int32 CompletionMinimumCharacters = 1;
    
    UPROPERTY(Config)
    FVector4f HistoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);

    virtual void Reset() override
    {
        Super::Reset();

        SortCommands = false;
        DockInputInMenuBar = false;
        FocusWidgetWhenAppearing = false;
        UseClipper = false;
        NumHistoryCommands = 10;
        CompletionMinimumCharacters = 1;
        HistoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);
    }
};