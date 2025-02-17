#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogInputActionInfo.h"
#include "CogWindow.h"
#include "imgui.h"
#include "InputCoreTypes.h"
#include "CogInputWindow_Gamepad.generated.h"

class UCogInputConfig_Gamepad;
class UCogInputDataAsset;
class UEnhancedPlayerInput;
struct FKey;
struct ImDrawList;

//--------------------------------------------------------------------------------------------------------------------------
class COGINPUT_API FCogInputWindow_Gamepad : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;

    virtual void PostBegin() override;

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaSeconds) override;

    virtual void RenderButton(const FKey& Key, const ImVec2& Position, const ImVec2& Size, const ImVec2& Alignment, float Rounding, ImDrawFlags Flags = 0);
    
    virtual void RenderStick(const FKey& Key2D, const FKey& KeyBool, bool& InvertY, float& Sensitivity, float Amplitude, const ImVec2& Position, float Radius);

    virtual void OnButtonClicked(FCogInputActionInfo* ActionInfo);

    virtual void RenderMainContextMenu();

    virtual void RenderButtonContextMenu(const FKey& Key, FCogInputActionInfo* ActionInfoButton);

    virtual void RenderStickContextMenu(const FKey& Key, FCogInputActionInfo* ActionInfo2D, bool& InvertY, float& Sensitivity);

    virtual void TryRefreshActions();

    static void ConstrainAspectRatio(ImGuiSizeCallbackData* InData);

    TObjectPtr<UCogInputConfig_Gamepad> Config;

    TMap<FKey, FCogInputActionInfo> Actions;
    
    UEnhancedPlayerInput* Input = nullptr;

    ImDrawList* DrawList = nullptr;
    
    float RepeatTime = 0.0f;
    
    ImVec2 CanvasMin;
    
    ImVec2 CanvasSize;
    
    ImVec2 CanvasMax;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogInputConfig_Gamepad : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bShowAsOverlay = false;

    UPROPERTY(Config)
    bool bLockPosition = false;

    UPROPERTY(Config)
    FVector2f Alignment = FVector2f(0.0f, 0.0f);

    UPROPERTY(Config)
    FIntVector2 Padding = FIntVector2(10, 10);
    
    UPROPERTY(Config)
    bool bInvertRightStickY = false;

    UPROPERTY(Config)
    bool bInvertLeftStickY = false;

    UPROPERTY(Config)
    float LeftStickSensitivity = 5.0f;

    UPROPERTY(Config)
    float RightStickSensitivity = 1.0f;

    UPROPERTY(Config)
    FVector4f BackgroundColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);

    UPROPERTY(Config)
    FVector4f ButtonColor = FVector4f(0.2f, 0.2f, 0.2f, 1.0f);

    UPROPERTY(Config)
    FVector4f BorderColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);

    UPROPERTY(Config)
    FVector4f PressedColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);

    UPROPERTY(Config)
    FVector4f HoveredColor = FVector4f(0.3f, 0.3f, 0.3f, 1.0f);

    UPROPERTY(Config)
    FVector4f InjectColor = FVector4f(1.0f, 0.5f, 0.0f, 0.5f);

    UPROPERTY(Config)
    float Border = 0.02f;

    UPROPERTY(Config)
    float RepeatPeriod = 0.5f;

    virtual void Reset() override
    {
        Super::Reset();

        bShowAsOverlay = false;
        bLockPosition = false;
        Alignment = { 0, 0 };
        Padding = { 10, 10 };
        bInvertRightStickY = false;
        bInvertLeftStickY = false;
        LeftStickSensitivity = 5.0f;
        RightStickSensitivity = 1.0f;
        BackgroundColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);
        ButtonColor = FVector4f(0.2f, 0.2f, 0.2f, 1.0f);
        BorderColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);
        PressedColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);
        HoveredColor = FVector4f(0.3f, 0.3f, 0.3f, 1.0f);
        InjectColor = FVector4f(1.0f, 0.5f, 0.0f, 0.5f);
        Border = 0.02f;
        RepeatPeriod = 0.5f;
    }
};