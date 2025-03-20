#include "CogInputWindow_Gamepad.h"

#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "imgui_internal.h"
#include "InputMappingContext.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::Initialize()
{
    Super::Initialize();

    bUseCustomContextMenu = true;

    Config = GetConfig<UCogInputConfig_Gamepad>();

    Actions.FindOrAdd(EKeys::Gamepad_Left2D);
    Actions.FindOrAdd(EKeys::Gamepad_LeftX);
    Actions.FindOrAdd(EKeys::Gamepad_LeftY);
    Actions.FindOrAdd(EKeys::Gamepad_Right2D);
    Actions.FindOrAdd(EKeys::Gamepad_RightX);
    Actions.FindOrAdd(EKeys::Gamepad_RightY);
    Actions.FindOrAdd(EKeys::Gamepad_LeftTriggerAxis);
    Actions.FindOrAdd(EKeys::Gamepad_RightTriggerAxis);
    Actions.FindOrAdd(EKeys::Gamepad_LeftThumbstick);
    Actions.FindOrAdd(EKeys::Gamepad_RightThumbstick);
    Actions.FindOrAdd(EKeys::Gamepad_Special_Left);
    Actions.FindOrAdd(EKeys::Gamepad_Special_Left_X);
    Actions.FindOrAdd(EKeys::Gamepad_Special_Left_Y);
    Actions.FindOrAdd(EKeys::Gamepad_Special_Right);
    Actions.FindOrAdd(EKeys::Gamepad_FaceButton_Bottom);
    Actions.FindOrAdd(EKeys::Gamepad_FaceButton_Right);
    Actions.FindOrAdd(EKeys::Gamepad_FaceButton_Left);
    Actions.FindOrAdd(EKeys::Gamepad_FaceButton_Top);
    Actions.FindOrAdd(EKeys::Gamepad_LeftShoulder);
    Actions.FindOrAdd(EKeys::Gamepad_RightShoulder);
    Actions.FindOrAdd(EKeys::Gamepad_LeftTrigger);
    Actions.FindOrAdd(EKeys::Gamepad_RightTrigger);
    Actions.FindOrAdd(EKeys::Gamepad_DPad_Up);
    Actions.FindOrAdd(EKeys::Gamepad_DPad_Down);
    Actions.FindOrAdd(EKeys::Gamepad_DPad_Right);
    Actions.FindOrAdd(EKeys::Gamepad_DPad_Left);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    Super::PreBegin(WindowFlags);

    WindowFlags |= ImGuiWindowFlags_NoScrollbar;
    
    if (Config->bShowAsOverlay)
    {
        WindowFlags = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBackground;
    }

    if (Config->bLockPosition)
    {
        ImVec2 WindowPos = FCogWidgets::ComputeScreenCornerLocation(Config->Alignment, Config->Padding);
        WindowPos.y -= Config->bShowAsOverlay && Config->Alignment.Y < 0.5f ? ImGui::GetFrameHeight() : 0.0f;
        ImGui::SetNextWindowPos(WindowPos, ImGuiCond_Always, FCogImguiHelper::ToImVec2(Config->Alignment));
    }

    ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX), FCogInputWindow_Gamepad::ConstrainAspectRatio); 
    ImGui::PushStyleColor(ImGuiCol_ResizeGrip, IM_COL32_BLACK_TRANS);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::ConstrainAspectRatio(ImGuiSizeCallbackData* InData)
{
    constexpr float AspectRatio = 0.60f;
    InData->DesiredSize.y = static_cast<int>(InData->DesiredSize.x * AspectRatio + ImGui::GetFrameHeight());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::PostBegin()
{
    Super::PostBegin();
    ImGui::PopStyleColor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::RenderButtonContextMenu(const FKey& Key, FCogInputActionInfo* ActionInfoButton)
{
    if (ActionInfoButton != nullptr)
    {
        ImGui::Separator();

        ImGui::Checkbox("Pressed", &ActionInfoButton->bPressed);
        ImGui::Checkbox("##Repeat", &ActionInfoButton->bRepeat);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(FCogWidgets::GetShortWidth() - ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);
        FCogWidgets::SliderWithReset("Repeat", &Config->RepeatPeriod, 0.0f, 10.0f, 0.5f, "%0.1fs");
    }

    if (ActionInfoButton != nullptr && ActionInfoButton->Action != nullptr && ActionInfoButton->Action->ValueType == EInputActionValueType::Axis1D)
    {
        ImGui::Separator();

        FCogWidgets::SetNextItemToShortWidth();
        FCogWidgets::SliderWithReset("X", &ActionInfoButton->X, -1.0f, 1.0f, 0.0f, "%0.2f");
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::RenderStickContextMenu(const FKey& Key, FCogInputActionInfo* ActionInfo2D, bool& InvertY, float& Sensitivity)
{
    if (ActionInfo2D != nullptr)
    {
        ImGui::Separator();

        FCogWidgets::SetNextItemToShortWidth();
        FCogWidgets::SliderWithReset("X", &ActionInfo2D->X, -1.0f, 1.0f, 0.0f, "%0.2f");
        FCogWidgets::SetNextItemToShortWidth();
        FCogWidgets::SliderWithReset("Y", &ActionInfo2D->Y, -1.0f, 1.0f, 0.0f, "%0.2f");
        ImGui::Checkbox("Invert Stick Y", &InvertY);
        FCogWidgets::SetNextItemToShortWidth();
        FCogWidgets::SliderWithReset("Sensitivity", &Sensitivity, 0.0f, 10.0f, 5.0f, "%0.1f");
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::OnButtonClicked(FCogInputActionInfo* ActionInfo)
{
    if (ActionInfo == nullptr)
    { return; }

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
void FCogInputWindow_Gamepad::RenderButton(const FKey& Key, const ImVec2& RelativePosition, const ImVec2& RelativeSize, const ImVec2& Alignment, float RelativeRounding, ImDrawFlags Flags)
{
    ImGui::PushID((void*)(&Key));

    const ImVec2& Size = RelativeSize * CanvasSize.x;
    const ImVec2 Position = (CanvasMin + CanvasSize * RelativePosition) - Alignment * Size;

    bool IsPressed = false;
    FCogInputActionInfo* ActionInfo = Actions.Find(Key);
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

    ImU32 Color = FCogImguiHelper::ToImU32(Config->ButtonColor);
    if (IsPressed)
    {
        Color = FCogImguiHelper::ToImU32(Config->PressedColor);
    }
    else if (ImGui::IsItemHovered())
    {
        Color = FCogImguiHelper::ToImU32(Config->HoveredColor);
    }

    if (ImGui::BeginPopupContextItem())
    {
        RenderMainContextMenu();
        ImGui::Separator();
        RenderButtonContextMenu(Key, ActionInfo);
        ImGui::EndPopup();
    }

    if (Config->Border > 0.0f)
    {
        const bool Inject = ActionInfo != nullptr && (ActionInfo->bPressed || ActionInfo->bRepeat);
        const ImU32 BColor = Inject ? FCogImguiHelper::ToImU32(Config->InjectColor) : FCogImguiHelper::ToImU32(Config->BorderColor);
        DrawList->AddRect(Position, Position + Size, BColor, RelativeRounding * CanvasSize.x, Flags, Config->Border * CanvasSize.x);
    }

    DrawList->AddRectFilled(Position, Position + Size, Color, RelativeRounding * CanvasSize.x, Flags);

    ImGui::PopID();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::RenderStick(const FKey& Key2D, const FKey& KeyBool, bool& InvertY, float& Sensitivity, float RelativeAmplitude, const ImVec2& RelativePosition, float RelativeRadius)
{
    ImGui::PushID((void*)(&Key2D));

    FCogInputActionInfo* ActionInfoBool = Actions.Find(KeyBool);
    FCogInputActionInfo* ActionInfo2D = Actions.Find(Key2D);

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
        if (ActionInfo2D != nullptr && ActionInfo2D->bIsMouseDraggingStick == false)
        {
            OnButtonClicked(ActionInfoBool);
        }
    }

    if (ActionInfo2D != nullptr && ActionInfo2D->Action != nullptr)
    {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            ActionInfo2D->bIsMouseDownOnStick = true;
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (ActionInfo2D->bIsMouseDraggingStick && ImGui::GetCurrentContext()->IO.KeyCtrl == false)
            {
                ActionInfo2D->X = 0.0f;
                ActionInfo2D->Y = 0.0f;
            }

            ActionInfo2D->bIsMouseDownOnStick = false;
            ActionInfo2D->bIsMouseDraggingStick = false;
        }

        if (ActionInfo2D->bIsMouseDownOnStick)
        {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 4.0f))
            {
                ActionInfo2D->bIsMouseDraggingStick = true;
                ImVec2 Delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left) * Sensitivity * 0.01f;
                const float Length = ImLengthSqr(Delta);
                if (Length > 1.0f)
                {
                    Delta = Delta / ImSqrt(Length);
                }
                
                ActionInfo2D->X = Delta.x;
                ActionInfo2D->Y = -Delta.y * (InvertY ? -1.0f : 1.0f);
            }
        }
    }

    ImU32 Color = FCogImguiHelper::ToImU32(Config->ButtonColor);
    if (Input->GetKeyValue(KeyBool) > 0.0f)
    {
        Color = FCogImguiHelper::ToImU32(Config->PressedColor);
    }
    else if (ImGui::IsItemHovered())
    {
        Color = FCogImguiHelper::ToImU32(Config->HoveredColor);
    }

    if (ImGui::BeginPopupContextItem())
    {
        RenderMainContextMenu();
        RenderButtonContextMenu(KeyBool, ActionInfoBool);
        RenderStickContextMenu(Key2D, ActionInfo2D, InvertY, Sensitivity);
        ImGui::EndPopup();
    }

    if (Config->Border > 0.0f)
    {
        const bool Inject = ActionInfoBool != nullptr && (ActionInfoBool->bPressed || ActionInfoBool->bRepeat);
        const ImU32 BColor = Inject ? FCogImguiHelper::ToImU32(Config->InjectColor) : FCogImguiHelper::ToImU32(Config->BorderColor);
        DrawList->AddCircle(StickPosition, Radius, BColor, 0, Config->Border * CanvasSize.x);
    }

    DrawList->AddCircleFilled(StickPosition, Radius, Color);

    ImGui::PopID();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::TryRefreshActions()
{
    const CogInputMappingContextMap* AppliedMappingContexts = &PRIVATE_ACCESS_PTR(Input, UEnhancedPlayerInput_AppliedInputContexts);
    
    if (AppliedMappingContexts == nullptr)
    { return; }

    if (AppliedMappingContexts->Num() == 0)
    { return; }

    for (auto& kv : *AppliedMappingContexts)
    {
        for (const FEnhancedActionKeyMapping& Mapping : kv.Key->GetMappings())
        {
            if (Mapping.Action != nullptr)
            {
                FCogInputActionInfo& ActionInfo = Actions.FindOrAdd(Mapping.Key);
                ActionInfo.Action = Mapping.Action;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::RenderContent()
{
    Super::RenderContent();

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer == nullptr)
    {
        ImGui::TextDisabled("No Local Player");
        return;
    }

    const UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (EnhancedInputSubsystem == nullptr)
    {
        ImGui::TextDisabled("No Enhanced Input Subsystem");
        return;
    }
    
    Input = EnhancedInputSubsystem->GetPlayerInput();
    if (Input == nullptr)
    {
        ImGui::TextDisabled("No Player Input");
        return;
    }

    TryRefreshActions();

    constexpr float AspectRatio = 0.55f;
    constexpr float StickAmplitude = 0.04f;
    constexpr float StickRadius = 0.08f;
    constexpr float StickBaseRadius = 0.1f;
    constexpr float GamepadRound = 0.15f;
    constexpr float HandleWidth = 0.26f;
    constexpr ImVec2 DPadButtonSize(0.08f, 0.08f);
    constexpr float DPadButtonDistance = 0.075f;
    constexpr float DPadButtonRounding = 0.1f;
    constexpr ImVec2 SpecialButtonSize(0.08f, 0.04f);
    constexpr float SpecialButtonRound = 0.05f;
    constexpr float SpecialButtonDistance = 0.15f;
    constexpr float TriggerRound = 0.02f;
    constexpr ImVec2 TriggerButtonSize(0.08f, 0.05f);
    constexpr float BumperRound = 0.02f;
    constexpr ImVec2 BumperButtonSize(0.2f, 0.04f);

    const ImVec2 ContentMin = ImGui::GetCursorScreenPos();
    const ImVec2 ContentSize = ImGui::GetContentRegionAvail();
    const ImVec2 ContentMax = ContentMin + ContentSize;
    const ImVec2 OverlayOffset = ImVec2(0.0f, Config->bShowAsOverlay && IsWindowRenderedInMainMenu() == false ? ImGui::GetFrameHeight() : 0.0f);
    
    const ImVec2 Padding = ImVec2(Config->Border * 0.5f * ContentSize.x, Config->Border * 0.5f * ContentSize.x);
    CanvasMin = ContentMin + OverlayOffset + Padding;
    CanvasSize = ImVec2(FMath::Max(ContentSize.x, 50.0f), ContentSize.x * AspectRatio) - (Padding * 2);
    CanvasMax = CanvasMin + CanvasSize + OverlayOffset;

    DrawList = ImGui::GetWindowDrawList();
    ImGui::Dummy(Config->bShowAsOverlay ? CanvasMax - CanvasMin : ContentSize);
    if (ImGui::BeginPopupContextItem("Gamepad"))
    {
        RenderMainContextMenu();
        ImGui::EndPopup();
    }

    //DrawList->PushClipRect(ContentMin, ContentMax, true);

    constexpr ImVec2 LS_Pos(0.35f, 0.7f);
    constexpr ImVec2 RS_Pos(0.65f, 0.7f);

    //------------------------------
    // Triggers
    //------------------------------
    RenderButton(EKeys::Gamepad_LeftTriggerAxis, ImVec2(0.18f, 0.12f), TriggerButtonSize, ImVec2(0.5f, 1.0f), TriggerRound, ImDrawFlags_RoundCornersTop);
    RenderButton(EKeys::Gamepad_RightTriggerAxis, ImVec2(1.f - 0.18f, 0.12f), TriggerButtonSize, ImVec2(0.5f, 1.0f), TriggerRound, ImDrawFlags_RoundCornersTop);

    //------------------------------
    // Shoulders
    //------------------------------
    RenderButton(EKeys::Gamepad_LeftShoulder, ImVec2(0.18f, 0.15f), BumperButtonSize, ImVec2(0.5f, 0.5f), BumperRound, ImDrawFlags_RoundCornersTop);
    RenderButton(EKeys::Gamepad_RightShoulder, ImVec2(1.f - 0.18f, 0.15f), BumperButtonSize, ImVec2(0.5f, 0.5f), BumperRound, ImDrawFlags_RoundCornersTop);

    //------------------------------
    // Gamepad
    //------------------------------
    if (Config->Border > 0.0f)
    {
        DrawList->AddRect(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 0.7f), FCogImguiHelper::ToImU32(Config->BorderColor), GamepadRound * CanvasSize.x, 0, Config->Border * CanvasSize.x);
        DrawList->AddRect(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(HandleWidth, 1.0f), FCogImguiHelper::ToImU32(Config->BorderColor), GamepadRound * CanvasSize.x, 0, Config->Border * CanvasSize.x);
        DrawList->AddRect(CanvasMin + CanvasSize * ImVec2(1.0f - HandleWidth, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 1.0f), FCogImguiHelper::ToImU32(Config->BorderColor), GamepadRound * CanvasSize.x, 0, Config->Border * CanvasSize.x);

        DrawList->AddCircle(CanvasMin + CanvasSize * LS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(Config->BorderColor), 0, Config->Border * CanvasSize.x);
        DrawList->AddCircle(CanvasMin + CanvasSize * RS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(Config->BorderColor), 0, Config->Border * CanvasSize.x);
    }

    DrawList->AddRectFilled(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 0.7f), FCogImguiHelper::ToImU32(Config->BackgroundColor), GamepadRound * CanvasSize.x);
    DrawList->AddRectFilled(CanvasMin + CanvasSize * ImVec2(0.0f, 0.16f), CanvasMin + CanvasSize * ImVec2(HandleWidth, 1.0f), FCogImguiHelper::ToImU32(Config->BackgroundColor), GamepadRound * CanvasSize.x);
    DrawList->AddRectFilled(CanvasMin + CanvasSize * ImVec2(1.0f - HandleWidth, 0.16f), CanvasMin + CanvasSize * ImVec2(1.0f, 1.0f), FCogImguiHelper::ToImU32(Config->BackgroundColor), GamepadRound * CanvasSize.x);
    
    DrawList->AddCircleFilled(CanvasMin + CanvasSize * LS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(Config->BackgroundColor));
    DrawList->AddCircleFilled(CanvasMin + CanvasSize * RS_Pos, StickBaseRadius* CanvasSize.x, FCogImguiHelper::ToImU32(Config->BackgroundColor));

    //------------------------------
    // Sticks
    //------------------------------
    RenderStick(EKeys::Gamepad_Left2D, EKeys::Gamepad_LeftThumbstick, Config->bInvertLeftStickY, Config->LeftStickSensitivity,  StickAmplitude, LS_Pos, StickRadius);
    RenderStick(EKeys::Gamepad_Right2D, EKeys::Gamepad_RightThumbstick, Config->bInvertRightStickY, Config->RightStickSensitivity, StickAmplitude, RS_Pos, StickRadius);

    //------------------------------
    // DPad Buttons
    //------------------------------
    constexpr ImVec2 DPad_Pos(0.15f, 0.44f);
    RenderButton(EKeys::Gamepad_DPad_Up,      DPad_Pos + ImVec2(0.0f, -DPadButtonDistance / AspectRatio), DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);
    RenderButton(EKeys::Gamepad_DPad_Down,    DPad_Pos + ImVec2(0.0f, DPadButtonDistance / AspectRatio),  DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);
    RenderButton(EKeys::Gamepad_DPad_Left,    DPad_Pos + ImVec2(-DPadButtonDistance, 0.0f), DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);
    RenderButton(EKeys::Gamepad_DPad_Right,   DPad_Pos + ImVec2(DPadButtonDistance, 0.0f),  DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);

    //------------------------------
    // Face Buttons
    //------------------------------
    constexpr ImVec2 Face_Pos(1.0f - 0.15f, 0.44f);
    RenderButton(EKeys::Gamepad_FaceButton_Top,       Face_Pos + ImVec2(0.0f, -DPadButtonDistance / AspectRatio), DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);
    RenderButton(EKeys::Gamepad_FaceButton_Bottom,    Face_Pos + ImVec2(0.0f, DPadButtonDistance / AspectRatio),  DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);
    RenderButton(EKeys::Gamepad_FaceButton_Left,      Face_Pos + ImVec2(-DPadButtonDistance, 0.0f), DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);
    RenderButton(EKeys::Gamepad_FaceButton_Right,     Face_Pos + ImVec2(DPadButtonDistance, 0.0f),  DPadButtonSize, ImVec2(0.5f, 0.5f), DPadButtonRounding);

    //------------------------------
    // Special Buttons
    //------------------------------
    RenderButton(EKeys::Gamepad_Special_Left,  ImVec2(0.5f - SpecialButtonDistance * 0.5f, 0.35f), SpecialButtonSize, ImVec2(0.5f, 0.5f), SpecialButtonRound);
    RenderButton(EKeys::Gamepad_Special_Right, ImVec2(0.5f + SpecialButtonDistance * 0.5f, 0.35f), SpecialButtonSize, ImVec2(0.5f, 0.5f), SpecialButtonRound);

    //DrawList->PopClipRect();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::RenderTick(float DeltaSeconds)
{
    Super::RenderTick(DeltaSeconds);

    if (GetWorld() == nullptr)
    { return; }

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer == nullptr)
    { return; }

    UEnhancedInputLocalPlayerSubsystem* EnhancedInput = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (EnhancedInput == nullptr)
    { return; }

    bool IsTimeToRepeat = false;
    const float WorldTime = GetWorld()->GetTimeSeconds();
    if (RepeatTime < WorldTime)
    {
        RepeatTime = WorldTime + Config->RepeatPeriod;
        IsTimeToRepeat = true;
    }

    for (auto& Entry : Actions)
    {
        FCogInputActionInfo& ActionInfo = Entry.Value;
        ActionInfo.Inject(*EnhancedInput, IsTimeToRepeat);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Gamepad::RenderMainContextMenu()
{
    if (ImGui::MenuItem("Close"))
    {
        SetIsVisible(false);
    }

    if (ImGui::BeginMenu("Display"))
    {
        ImGui::Checkbox("Overlay", &Config->bShowAsOverlay);
        ImGui::Checkbox("Lock Position", &Config->bLockPosition);
        ImGui::BeginDisabled(!Config->bLockPosition);
        FCogWidgets::SetNextItemToShortWidth();
        ImGui::SliderFloat2("Alignment", &Config->Alignment.X, 0, 1.0f, "%.2f");
        FCogWidgets::SetNextItemToShortWidth();
        ImGui::SliderInt2("Padding", &Config->Padding.X, 0, 100);
        ImGui::EndDisabled();
        
        ImGui::Separator();

        ImGui::ColorEdit4("Background Color", &Config->BackgroundColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Border Color", &Config->BorderColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Button Color", &Config->ButtonColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Pressed Color", &Config->PressedColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Hovered Color", &Config->HoveredColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Inject Color", &Config->InjectColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWidgets::SetNextItemToShortWidth();
        FCogWidgets::SliderWithReset("Border", &Config->Border, 0.0f, 0.1f, 0.02f, "%0.3f");
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Reset Settings"))
    {
        ResetConfig();
    }
}