#include "CogInputWindow_Gamepad.h"

#include "CogImguiHelper.h"
#include "CogInputDataAsset.h"
#include "CogWindowHelper.h"
#include "CogWindowWidgets.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "imgui_internal.h"
#include "InputMappingContext.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogInputWindow_Gamepad::UCogInputWindow_Gamepad()
{
    Asset = FCogWindowHelper::GetFirstAssetByClass<UCogInputDataAsset>();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::PreRender(ImGuiWindowFlags& WindowFlags)
{
    if (bShowAsOverlay)
    {
        WindowFlags = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBackground
            | ImGuiWindowFlags_NoResize;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::ResetConfig()
{
    Super::ResetConfig();

    bShowAsOverlay = false;
    bInvertRightStickY = false;
    bInvertLeftStickY = false;
    BackgroundColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);
    ButtonColor = FVector4f(0.2f, 0.2f, 0.2f, 1.0f);
    BorderColor = FVector4f(0.03f, 0.03f, 0.03f, 1.0f);
    PressedColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);
    HoveredColor = FVector4f(0.3f, 0.3f, 0.3f, 1.0f);
    InjectColor = FVector4f(1.0f, 0.5f, 0.0f, 0.5f);
    Border = 0.02f;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::InputContextMenu(const FKey& Key, FCogInjectActionInfo* ActionInfoButton, FCogInjectActionInfo* ActionInfo2D)
{
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text("%s", TCHAR_TO_ANSI(*Key.GetFName().ToString()));
        ImGui::Separator();

        if (ActionInfoButton != nullptr)
        {
            ImGui::Checkbox("Pressed", &ActionInfoButton->bPressed);
            ImGui::Checkbox("Repeat", &ActionInfoButton->bRepeat);
            FCogWindowWidgets::SliderWithReset("Period", &RepeatPeriod, 0.0f, 10.0f, 0.5f, "%0.1fs");
        }

        if (ActionInfoButton != nullptr && ActionInfoButton->Action != nullptr && ActionInfoButton->Action->ValueType == EInputActionValueType::Axis1D)
        {
            FCogWindowWidgets::SliderWithReset("X", &ActionInfoButton->X, -1.0f, 1.0f, 0.0f, "%0.2f");
        }

        if (ActionInfo2D != nullptr)
        {
            FCogWindowWidgets::SliderWithReset("X", &ActionInfo2D->X, -1.0f, 1.0f, 0.0f, "%0.2f");
            FCogWindowWidgets::SliderWithReset("Y", &ActionInfo2D->Y, -1.0f, 1.0f, 0.0f, "%0.2f");
        }
    
        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::OnButtonClicked(FCogInjectActionInfo* ActionInfo)
{
    if (ActionInfo == nullptr)
    {
        return;
    }

    if (ActionInfo->bRepeat)
    {
        ActionInfo->bRepeat = false;
    }
    else
    {
        ActionInfo->bPressed = !ActionInfo->bPressed;
    }

    if (ActionInfo->Action != nullptr)
    {
        Input->InjectInputForAction(ActionInfo->Action, FInputActionValue(ActionInfo->bPressed), {}, {});
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::AddButton(const FKey& Key, const ImVec2& RelativePosition, const ImVec2& RelativeSize, const ImVec2& Alignment, float RelativeRounding, ImDrawFlags Flags)
{
    ImGui::PushID((void*)(&Key));

    const float Value = Input->GetKeyValue(Key);

    const ImVec2& Size = RelativeSize * CanvasSize.x;
    const ImVec2 Position = (CanvasMin + CanvasSize * RelativePosition) - Alignment * Size;

    bool IsPressed = false;
    FCogInjectActionInfo* ActionInfo = Actions.Find(Key);
    if (ActionInfo != nullptr)
    {
        if (ActionInfo->Action != nullptr)
        {
            if (Input->GetActionValue(ActionInfo->Action).Get<bool>())
            {
                IsPressed = true;
            }
        }
    }

    ImGui::SetCursorScreenPos(Position);
    if (ImGui::InvisibleButton("", Size))
    {
        OnButtonClicked(ActionInfo);
    }

    ImU32 Color = FCogImguiHelper::ToImU32(ButtonColor);
    if (IsPressed)
    {
        Color = FCogImguiHelper::ToImU32(PressedColor);
    }
    else if (ImGui::IsItemHovered())
    {
        Color = FCogImguiHelper::ToImU32(HoveredColor);
    }

    InputContextMenu(Key, ActionInfo, nullptr);

    if (Border > 0.0f)
    {
        const bool Inject = ActionInfo != nullptr && (ActionInfo->bPressed || ActionInfo->bRepeat);
        const ImU32 BColor = Inject ? FCogImguiHelper::ToImU32(InjectColor) : FCogImguiHelper::ToImU32(BorderColor);
        DrawList->AddRect(Position, Position + Size, BColor, RelativeRounding * CanvasSize.x, Flags, Border * CanvasSize.x);
    }

    DrawList->AddRectFilled(Position, Position + Size, Color, RelativeRounding * CanvasSize.x, Flags);

    ImGui::PopID();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::AddStick(const FKey& Key2D, const FKey& KeyBool, bool InvertY, float RelativeAmplitude, const ImVec2& RelativePosition, float RelativeRadius)
{
    ImGui::PushID((void*)(&Key2D));

    FCogInjectActionInfo* ActionInfoBool = Actions.Find(KeyBool);
    FCogInjectActionInfo* ActionInfo2D = Actions.Find(Key2D);

    FVector Value = Input->GetRawVectorKeyValue(Key2D);
    if (ActionInfo2D != nullptr && ActionInfo2D->Action != nullptr)
    {
        Value = Input->GetActionValue(ActionInfo2D->Action).Get<FVector>();
    }

    Value.X = FMath::Clamp(Value.X, -1.0f, 1.0f);
    Value.Y = FMath::Clamp(Value.Y, -1.0f, 1.0f);

    const ImVec2 StickPosition = (CanvasMin + CanvasSize * RelativePosition) + ImVec2(Value.X, -Value.Y * (InvertY ? -1.0f : 1.0f)) * RelativeAmplitude * CanvasSize.x;
    const float Radius = RelativeRadius * CanvasSize.x;
    const ImVec2 ButtonPosition = StickPosition - ImVec2(Radius, Radius);
    const ImVec2 ButtonSize(2 * Radius, 2 * Radius);

    ImGui::SetCursorScreenPos(ButtonPosition);
    if (ImGui::InvisibleButton("", ButtonSize))
    {
        OnButtonClicked(ActionInfoBool);
    }

    ImU32 Color = FCogImguiHelper::ToImU32(ButtonColor);
    if (Input->GetKeyValue(KeyBool) > 0.0f)
    {
        Color = FCogImguiHelper::ToImU32(PressedColor);
    }
    else if (ImGui::IsItemHovered())
    {
        Color = FCogImguiHelper::ToImU32(HoveredColor);
    }

    InputContextMenu(Key2D, ActionInfoBool, ActionInfo2D);

    if (Border > 0.0f)
    {
        const bool Inject = ActionInfoBool != nullptr && (ActionInfoBool->bPressed || ActionInfoBool->bRepeat);
        const ImU32 BColor = Inject ? FCogImguiHelper::ToImU32(InjectColor) : FCogImguiHelper::ToImU32(BorderColor);
        DrawList->AddCircle(StickPosition, Radius, BColor, 0, Border * CanvasSize.x);
    }

    DrawList->AddCircleFilled(StickPosition, Radius, Color);

    ImGui::PopID();
}


//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::RenderContent()
{
    Super::RenderContent();

    if (GetWorld() == nullptr)
    {
        ImGui::Text("No World");
        return;
    }

    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer == nullptr)
    {
        ImGui::Text("No Local Player");
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (EnhancedInputSubsystem == nullptr)
    {
        ImGui::Text("No Enhanced Input Subsystem");
        return;
    }
    
    Input = EnhancedInputSubsystem->GetPlayerInput();
    if (Input == nullptr)
    {
        ImGui::Text("No Player Input");
        return;
    }

    if (Asset == nullptr)
    {
        ImGui::Text("No Input Asset");
    }
    else
    {
        if (Actions.Num() == 0)
        {
            Actions.FindOrAdd(EKeys:: Gamepad_Left2D);
            Actions.FindOrAdd(EKeys:: Gamepad_LeftX);
            Actions.FindOrAdd(EKeys:: Gamepad_LeftY);
            Actions.FindOrAdd(EKeys:: Gamepad_Right2D);
            Actions.FindOrAdd(EKeys:: Gamepad_RightX);
            Actions.FindOrAdd(EKeys:: Gamepad_RightY);
            Actions.FindOrAdd(EKeys:: Gamepad_LeftTriggerAxis);
            Actions.FindOrAdd(EKeys:: Gamepad_RightTriggerAxis);
            Actions.FindOrAdd(EKeys:: Gamepad_LeftThumbstick);
            Actions.FindOrAdd(EKeys:: Gamepad_RightThumbstick);
            Actions.FindOrAdd(EKeys:: Gamepad_Special_Left);
            Actions.FindOrAdd(EKeys:: Gamepad_Special_Left_X);
            Actions.FindOrAdd(EKeys:: Gamepad_Special_Left_Y);
            Actions.FindOrAdd(EKeys:: Gamepad_Special_Right);
            Actions.FindOrAdd(EKeys:: Gamepad_FaceButton_Bottom);
            Actions.FindOrAdd(EKeys:: Gamepad_FaceButton_Right);
            Actions.FindOrAdd(EKeys:: Gamepad_FaceButton_Left);
            Actions.FindOrAdd(EKeys:: Gamepad_FaceButton_Top);
            Actions.FindOrAdd(EKeys:: Gamepad_LeftShoulder);
            Actions.FindOrAdd(EKeys:: Gamepad_RightShoulder);
            Actions.FindOrAdd(EKeys:: Gamepad_LeftTrigger);
            Actions.FindOrAdd(EKeys:: Gamepad_RightTrigger);
            Actions.FindOrAdd(EKeys:: Gamepad_DPad_Up);
            Actions.FindOrAdd(EKeys:: Gamepad_DPad_Down);
            Actions.FindOrAdd(EKeys:: Gamepad_DPad_Right);
            Actions.FindOrAdd(EKeys:: Gamepad_DPad_Left);

            for (TObjectPtr<const UInputMappingContext> MappingContext : Asset->MappingContexts)
            {
                for (const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
                {
                    if (Mapping.Action != nullptr)
                    {
                        FCogInjectActionInfo& ActionInfo = Actions.FindOrAdd(Mapping.Key);
                        ActionInfo.Action = Mapping.Action;
                    }
                }
            }
        }
    }


    const float AspectRatio = 0.55f;
    const float StickAmplitude = 0.04f;
    const float StickRadius = 0.08f;
    const float StickBaseRadius = 0.1f;
    const float GamepadRound = 0.15f;
    const float HandleWidth = 0.26f;
    const ImVec2 DPadButtonSize(0.08f, 0.08f);
    const float DpadButtonDistance = 0.075f;
    const float DpadButtonRounding = 0.1f;
    const ImVec2 SpecialButtonSize(0.08f, 0.04f);
    const float SpecialButtonRound = 0.05f;
    const float SpecialButtonDistance = 0.15f;
    const float TriggerRound = 0.02f;
    const ImVec2 TriggerButtonSize(0.08f, 0.05f);
    const float BumperRound = 0.02f;
    const ImVec2 BumperButtonSize(0.2f, 0.04f);

    const ImVec2 ContentMin = ImGui::GetCursorScreenPos();
    const ImVec2 ContentSize = ImGui::GetContentRegionAvail();
    const ImVec2 ContentMax = ContentMin + ContentSize;
    const ImVec2 OverlayOffset = ImVec2(0.0f, bShowAsOverlay ? ImGui::GetFrameHeight() : 0.0f);
    
    const ImVec2 Padding = ImVec2(Border * 0.5f * ContentSize.x, Border * 0.5f * ContentSize.x);
    CanvasMin = ContentMin + OverlayOffset + Padding;
    CanvasSize = ImVec2(FMath::Max(ContentSize.x, 50.0f), ContentSize.x * AspectRatio) - (Padding * 2);
    CanvasMax = CanvasMin + CanvasSize + OverlayOffset;

    DrawList = ImGui::GetWindowDrawList();
    ImGui::Dummy(bShowAsOverlay ? CanvasMax - CanvasMin : ContentSize);
    if (ImGui::BeginPopupContextItem("Gamepad"))
    {
        if (ImGui::Button("Close", ImVec2(-1.0f, 0)))
        {
            SetIsVisible(false);
        }
        ImGui::Checkbox("Overlay", &bShowAsOverlay);
        ImGui::Separator();
        ImGui::Checkbox("Invert Left Stick Y", &bInvertLeftStickY);
        ImGui::Checkbox("Invert Right Stick Y", &bInvertRightStickY);
        ImGui::Separator();
        ImGui::ColorEdit4("Background Color", (float*)&BackgroundColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Border Color", (float*)&BorderColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Button Color", (float*)&ButtonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Pressed Color", (float*)&PressedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Hovered Color", (float*)&HoveredColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Inject Color", (float*)&InjectColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::SliderWithReset("Border", &Border, 0.0f, 0.1f, 0.02f, "%0.3f");
        ImGui::EndPopup();
    }

    DrawList->PushClipRect(ContentMin, ContentMax, true);

    const ImVec2 LS_Pos(0.35f, 0.7f);
    const ImVec2 RS_Pos(0.65f, 0.7f);

    //------------------------------
    // Triggers
    //------------------------------
    AddButton(EKeys::Gamepad_LeftTriggerAxis, ImVec2(0.18f, 0.12f), TriggerButtonSize, ImVec2(0.5f, 1.0f), TriggerRound, ImDrawFlags_RoundCornersTop);
    AddButton(EKeys::Gamepad_RightTriggerAxis, ImVec2(1.f - 0.18f, 0.12f), TriggerButtonSize, ImVec2(0.5f, 1.0f), TriggerRound, ImDrawFlags_RoundCornersTop);

    //------------------------------
    // Shoulders
    //------------------------------
    AddButton(EKeys::Gamepad_LeftShoulder, ImVec2(0.18f, 0.15f), BumperButtonSize, ImVec2(0.5f, 0.5f), BumperRound, ImDrawFlags_RoundCornersTop);
    AddButton(EKeys::Gamepad_RightShoulder, ImVec2(1.f - 0.18f, 0.15f), BumperButtonSize, ImVec2(0.5f, 0.5f), BumperRound, ImDrawFlags_RoundCornersTop);

    //------------------------------
    // Gamepad
    //------------------------------
    if (Border > 0.0f)
    {
        DrawList->AddRect(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 0.7f), FCogImguiHelper::ToImU32(BorderColor), GamepadRound * CanvasSize.x, 0, Border * CanvasSize.x);
        DrawList->AddRect(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(HandleWidth, 1.0f), FCogImguiHelper::ToImU32(BorderColor), GamepadRound * CanvasSize.x, 0, Border * CanvasSize.x);
        DrawList->AddRect(CanvasMin + CanvasSize * ImVec2(1.0f - HandleWidth, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 1.0f), FCogImguiHelper::ToImU32(BorderColor), GamepadRound * CanvasSize.x, 0, Border * CanvasSize.x);

        DrawList->AddCircle(CanvasMin + CanvasSize * LS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(BorderColor), 0, Border* CanvasSize.x);
        DrawList->AddCircle(CanvasMin + CanvasSize * RS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(BorderColor), 0, Border* CanvasSize.x);
    }

    DrawList->AddRectFilled(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 0.7f), FCogImguiHelper::ToImU32(BackgroundColor), GamepadRound * CanvasSize.x);
    DrawList->AddRectFilled(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(HandleWidth, 1.0f), FCogImguiHelper::ToImU32(BackgroundColor), GamepadRound * CanvasSize.x);
    DrawList->AddRectFilled(CanvasMin + CanvasSize * ImVec2(1.0f - HandleWidth, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 1.0f), FCogImguiHelper::ToImU32(BackgroundColor), GamepadRound * CanvasSize.x);
    
    DrawList->AddCircleFilled(CanvasMin + CanvasSize * LS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(BackgroundColor));
    DrawList->AddCircleFilled(CanvasMin + CanvasSize * RS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(BackgroundColor));

    //------------------------------
    // Sticks
    //------------------------------
    AddStick(EKeys::Gamepad_Left2D, EKeys::Gamepad_LeftThumbstick, bInvertLeftStickY, StickAmplitude, LS_Pos, StickRadius);
    AddStick(EKeys::Gamepad_Right2D, EKeys::Gamepad_RightThumbstick, bInvertRightStickY, StickAmplitude, RS_Pos, StickRadius);

    //------------------------------
    // DPad Buttons
    //------------------------------
    const ImVec2 DPad_Pos(0.15f, 0.44f);
    AddButton(EKeys::Gamepad_DPad_Up,      DPad_Pos + ImVec2(0.0f, -DpadButtonDistance / AspectRatio), DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);
    AddButton(EKeys::Gamepad_DPad_Down,    DPad_Pos + ImVec2(0.0f, DpadButtonDistance / AspectRatio),  DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);
    AddButton(EKeys::Gamepad_DPad_Left,    DPad_Pos + ImVec2(-DpadButtonDistance, 0.0f), DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);
    AddButton(EKeys::Gamepad_DPad_Right,   DPad_Pos + ImVec2(DpadButtonDistance, 0.0f),  DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);

    //------------------------------
    // Face Buttons
    //------------------------------
    const ImVec2 Face_Pos(1.0f - 0.15f, 0.44f);
    AddButton(EKeys::Gamepad_FaceButton_Top,       Face_Pos + ImVec2(0.0f, -DpadButtonDistance / AspectRatio), DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);
    AddButton(EKeys::Gamepad_FaceButton_Bottom,    Face_Pos + ImVec2(0.0f, DpadButtonDistance / AspectRatio),  DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);
    AddButton(EKeys::Gamepad_FaceButton_Left,      Face_Pos + ImVec2(-DpadButtonDistance, 0.0f), DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);
    AddButton(EKeys::Gamepad_FaceButton_Right,     Face_Pos + ImVec2(DpadButtonDistance, 0.0f),  DPadButtonSize, ImVec2(0.5f, 0.5f), DpadButtonRounding);

    //------------------------------
    // Special Buttons
    //------------------------------
    AddButton(EKeys::Gamepad_Special_Left,  ImVec2(0.5f - SpecialButtonDistance * 0.5f, 0.35f), SpecialButtonSize, ImVec2(0.5f, 0.5f), SpecialButtonRound);
    AddButton(EKeys::Gamepad_Special_Right, ImVec2(0.5f + SpecialButtonDistance * 0.5f, 0.35f), SpecialButtonSize, ImVec2(0.5f, 0.5f), SpecialButtonRound);

    DrawList->PopClipRect();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogInputWindow_Gamepad::RenderTick(float DeltaSeconds)
{
    Super::RenderTick(DeltaSeconds);

    if (GetWorld() == nullptr)
    {
        return;
    }

    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer == nullptr)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* EnhancedInput = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (EnhancedInput == nullptr)
    {
        return;
    }

    bool IsTimeToRepeat = false;
    float WorldTime = GetWorld()->GetTimeSeconds();
    if (RepeatTime < WorldTime)
    {
        RepeatTime = WorldTime + RepeatPeriod;
        IsTimeToRepeat = true;
    }

    for (auto& Entry : Actions)
    {
        FCogInjectActionInfo& ActionInfo = Entry.Value;
        ActionInfo.Inject(*EnhancedInput, IsTimeToRepeat);
    }
}