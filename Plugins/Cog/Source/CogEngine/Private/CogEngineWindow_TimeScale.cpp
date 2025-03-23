#include "CogEngineWindow_TimeScale.h"

#include "CogEngineReplicator.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogSubsystem.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "Engine/World.h"



//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::Initialize()
{
    Super::Initialize();

    bHasWidget = true;
    bIsWidgetVisible = true;
    
    Config = GetConfig<UCogEngineWindowConfig_TimeScale>();

    UCogEngineWindowConfig_TimeScale* ConfigPtr = Config.Get();
    GetOwner()->AddShortcut(ConfigPtr, &UCogEngineWindowConfig_TimeScale::Shortcut_FasterTimeScale).BindRaw(this, &FCogEngineWindow_TimeScale::FasterTimeScale);
    GetOwner()->AddShortcut(ConfigPtr, &UCogEngineWindowConfig_TimeScale::Shortcut_SlowerTimeScale).BindRaw(this, &FCogEngineWindow_TimeScale::SlowerTimeScale);
    GetOwner()->AddShortcut(ConfigPtr, &UCogEngineWindowConfig_TimeScale::Shortcut_ResetTimeScale).BindRaw(this, &FCogEngineWindow_TimeScale::ResetTimeScale);
    GetOwner()->AddShortcut(ConfigPtr, &UCogEngineWindowConfig_TimeScale::Shortcut_ZeroTimeScale).BindRaw(this, &FCogEngineWindow_TimeScale::ZeroTimeScale);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderHelp()
{
    ImGui::Text(
        "This window can be used to change the game global time scale. "
        "If changed on a client the time scale is also modified on the game server. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderContent()
{
    Super::RenderContent();

    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        ImGui::TextDisabled("Invalid Replicator");
        return;
    }

    RenderTimeScaleChoices(Replicator);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderContextMenu()
{
    UCogEngineWindowConfig_TimeScale* ConfigPtr = Config.Get();
    
    if (IsWindowRenderedInMainMenu() == false)
    {
        ImGui::Checkbox("Inline", &ConfigPtr->Inline);
    }

    FCogImguiHelper::ColorEdit4("Time Scale Modified Color", ConfigPtr->TimeScaleModifiedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::SetItemTooltip("Color of the current time scale, in widget mode, when the time scale in not 1.");
    
    FCogWidgets::FloatArray("Time Scales", ConfigPtr->TimeScales, 10, ImVec2(0, ImGui::GetFontSize() * 10));

    if (ImGui::CollapsingHeader("Shortcuts", ImGuiTreeNodeFlags_DefaultOpen))
    {
        RenderConfigShortcuts(*ConfigPtr);
    }

    ImGui::Separator();
    FCogWindow::RenderContextMenu();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderMainMenuWidget()
{
    Super::RenderMainMenuWidget();
    
    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        ImGui::TextDisabled("x?");
        ImGui::SetItemTooltip("Invalid Replicator");
        return;
    }

    float TimeDilation = GetTimeDilation();
    if (TimeDilation != 1.0f)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(Config->TimeScaleModifiedColor));
    }

    if (FMath::IsNearlyZero(TimeDilation, 0.0001f))
    {
        TimeDilation = 0.0f;
    }
    
    const auto Text = StringCast<ANSICHAR>(*FString::Printf(TEXT("x%g"), TimeDilation));
    const bool Open = ImGui::BeginMenu(Text.Get());
    
    if (TimeDilation != 1)
    {
        ImGui::PopStyleColor();
    }
    
    if (ImGui::BeginPopupContextItem())
    {
        RenderContextMenu();
        ImGui::EndPopup();
    }

    if (Open)
    {
        for (int32 i = 0; i < Config->TimeScales.Num(); ++i)
        {
            const float Value = Config->TimeScales[i];
            const auto ValueText = StringCast<ANSICHAR>(*FString::Printf(TEXT("%g"), Value));
            if (ImGui::Selectable(ValueText.Get(), Value == TimeDilation))
            {
                SetCurrentTimeScale(*Replicator, Value);
            }
        }
        
        ImGui::EndMenu();
    }
    else
    {
        if (FCogWidgets::BeginItemTooltipWrappedText())
        {
            ImGui::Text("Time Scale: x%g", TimeDilation);
            ImGui::Spacing();
            ImGui::Separator();
            FCogWidgets::TextOfAllInputChordsOfConfig(*Config.Get());
            FCogWidgets::EndItemTooltipWrappedText();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
int32 FCogEngineWindow_TimeScale::GetCurrentTimeScaleIndex(const ACogEngineReplicator& Replicator) const
{
    return GetTimeScaleIndex(Replicator.GetTimeDilation());
}

//--------------------------------------------------------------------------------------------------------------------------
int32 FCogEngineWindow_TimeScale::GetTimeScaleIndex(float InTimeScale) const
{
    for (int32 i = 0; i < Config->TimeScales.Num(); ++i)
    {
        const float Value = Config->TimeScales[i];
        if (FMath::IsNearlyEqual(Value, InTimeScale))
        { return i; }
    }

    return INDEX_NONE;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::SetCurrentTimeScale(ACogEngineReplicator& Replicator, const float Value) const
{
    Replicator.SetTimeDilation(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::SetCurrentTimeScaleIndex(ACogEngineReplicator& Replicator, int32 InTimeScaleIndex) const
{
    if (Config->TimeScales.IsValidIndex(InTimeScaleIndex) == false)
    { return; }

    const float Value = Config->TimeScales[InTimeScaleIndex];

    SetCurrentTimeScale(Replicator, Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::FasterTimeScale()
{
    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    { return; }
    
    const int32 TimeScaleIndex = GetCurrentTimeScaleIndex(*Replicator);
    if (TimeScaleIndex == INDEX_NONE)
    { return; }

    SetCurrentTimeScaleIndex(*Replicator, TimeScaleIndex + 1);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::SlowerTimeScale()
{
    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    { return; }
    
    const int32 TimeScaleIndex = GetCurrentTimeScaleIndex(*Replicator);
    if (TimeScaleIndex == INDEX_NONE)
    { return; }

    SetCurrentTimeScaleIndex(*Replicator, TimeScaleIndex - 1);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::ResetTimeScale()
{
    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    { return; }
    
    SetCurrentTimeScale(*Replicator, 1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::ZeroTimeScale()
{
    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    { return; }
    
    SetCurrentTimeScale(*Replicator, 0.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogEngineWindow_TimeScale::GetTimeDilation() const
{
    const UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return 1.0f;
    }

    AWorldSettings* WorldSettings = World->GetWorldSettings();
    if (WorldSettings == nullptr)
    {
        return 1.0f;
    }

    return WorldSettings->GetEffectiveTimeDilation();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderTimeScaleChoices(ACogEngineReplicator* Replicator)
{
    float TimeDilation = GetTimeDilation();
    if (FCogWidgets::MultiChoiceButtonsFloat(Config->TimeScales, TimeDilation, ImVec2(3.5f * FCogWidgets::GetFontWidth(), 0), Config->Inline, 0.0001f))
    {
        Replicator->SetTimeDilation(TimeDilation);
    }
}
