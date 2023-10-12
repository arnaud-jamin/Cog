#pragma once

#include "CoreMinimal.h"
#include "CogInjectActionInfo.h"
#include "CogWindow.h"
#include "CogInputWindow_Gamepad.generated.h"

class UEnhancedPlayerInput;
class UCogInputDataAsset;
struct ImDrawList;

UCLASS(Config = Cog)
class COGINPUT_API UCogInputWindow_Gamepad : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogInputWindow_Gamepad();

    const UCogInputDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogInputDataAsset* Value) { Asset = Value; }

protected:

    virtual void ResetConfig() override;

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaSeconds) override;

private:
    
    void AddButton(const FKey& Key, const ImVec2& Position, const ImVec2& Size, const ImVec2& Alignment, float Rounding, ImDrawFlags Flags = 0);
    
    void AddStick(const FKey& Key2D, const FKey& KeyBool, bool InvertY, float Amplitude, const ImVec2& Position, float Radius);
    
    void InputContextMenu(const FKey& Key, FCogInjectActionInfo* ActionInfoBool, FCogInjectActionInfo* ActionInfo2D);
    
    void OnButtonClicked(FCogInjectActionInfo* ActionInfo);

    UPROPERTY(Config)
    bool bShowAsOverlay = false;

    UPROPERTY(Config)
    bool bInvertRightStickY = false;

    UPROPERTY(Config)
    bool bInvertLeftStickY = false;

    UPROPERTY(Config)
    FVector4f BackgroundColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);

    UPROPERTY(Config)
    FVector4f ButtonColor = FVector4f(0.2f, 0.2f, 0.2f, 1.0f);

    UPROPERTY(Config)
    FVector4f BorderColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);

    UPROPERTY(Config)
    FVector4f PressedColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);

    UPROPERTY(Config)
    FVector4f InjectColor = FVector4f(1.0f, 0.5f, 0.0f, 0.5f);

    UPROPERTY(Config)
    float Border = 0.02f;

    UPROPERTY()
    TWeakObjectPtr<const UCogInputDataAsset> Asset;

    TMap<FKey, FCogInjectActionInfo> Actions;
    
    UEnhancedPlayerInput* Input = nullptr;

    ImDrawList* DrawList = nullptr;
    
    float RepeatPeriod = 0.5f;
    
    float RepeatTime = 0.0f;
    
    ImVec2 CanvasMin;
    
    ImVec2 CanvasSize;
    
    ImVec2 CanvasMax;
};
