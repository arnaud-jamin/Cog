#include "CogEngineWindow_Notifications.h"

#include "CogCommon.h"
#include "CogCommonLogCategory.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "Misc/StringBuilder.h"

int32 FCogEngineWindow_Notifications::NotificationsId = 0;

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::Initialize()
{    
    Super::Initialize();

    Config = GetConfig<UCogEngineConfig_Notifications>();

    OutputDevice.Notifications = this;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderHelp()
{
    ImGui::Text("This window manage the notifications.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::Clear()
{
    Notifications.Empty();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::AddNotification(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity)
{
    FNotification& Notification = Notifications.AddDefaulted_GetRef();
    Notification.Id = FString::Printf(TEXT("###Notify%d"), NotificationsId);
    Notification.Time = FDateTime::Now();
    Notification.Verbosity = InVerbosity;
    Notification.Message = InMessage;

    NotificationsId++;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::OnLogReceived(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity, const class FName& InCategory)
{
    if (Config == nullptr)
    { return; }

    if (Config->DisableNotifications)
    { return; }
    
    static FName CmdName("Cmd");

#if ENABLE_COG
    if (InCategory == LogCogNotify.GetCategoryName()
        || (InCategory == CmdName && Config->NotifyConsoleCommands)
        || (InVerbosity == ELogVerbosity::Warning && Config->NotifyAllWarnings)
        || (InVerbosity == ELogVerbosity::Error && Config->NotifyAllErrors))
    {
        AddNotification(InMessage, InVerbosity);
    }
#endif
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (Config->DisableNotifications)
    { return; }
    
    RenderNotifications();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderNotifications()
{
    if (Notifications.Num() == 0)
    { return; }

    constexpr ImGuiWindowFlags Flags =
        ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav
        | ImGuiWindowFlags_NoInputs;

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();

    const float DpiScale = GetDpiScale();
    
    ImVec2 WindowPos = FCogWidgets::ComputeScreenCornerLocation(Config->Alignment, Config->Padding);
    const ImVec2 WindowPadding = ImGui::GetStyle().WindowPadding;
    const ImVec2 ItemSpacing = ImGui::GetStyle().ItemSpacing;
    const float MaxHeight = Config->MaxHeight * DpiScale + WindowPadding.y * 2;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Config->Rounding); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Config->ShowBorder); 

    const FDateTime Now = FDateTime::Now();
    
    for (int32 i = Notifications.Num() - 1; i >= 0; i--)
    {
        const FNotification& Notification = Notifications[i];

        const FTimespan Span = Now - Notification.Time;
        if (Span > FTimespan::FromSeconds(Config->Duration + Config->FadeOut))
        {
            Notifications.RemoveAt(i);
            continue;
        }
        
        ImVec4 TextColor, BackColor, BorderColor;
        switch (Notification.Verbosity)
        {
            case ELogVerbosity::Error:
            {
                TextColor = FCogImguiHelper::ToImVec4(Config->TextErrorColor);
                BackColor = FCogImguiHelper::ToImVec4(Config->BackgroundErrorColor);
                BorderColor = FCogImguiHelper::ToImVec4(Config->BorderErrorColor);
                break;
            }
                
            case ELogVerbosity::Warning:
            {
                TextColor = FCogImguiHelper::ToImVec4(Config->TextWarningColor); 
                BackColor = FCogImguiHelper::ToImVec4(Config->BackgroundWarningColor);
                BorderColor = FCogImguiHelper::ToImVec4(Config->BorderWarningColor);
                break;
            }
                
            default:
            {
                TextColor = FCogImguiHelper::ToImVec4(Config->TextDefaultColor);
                BackColor = FCogImguiHelper::ToImVec4(Config->BackgroundDefaultColor);
                BorderColor = FCogImguiHelper::ToImVec4(Config->BorderDefaultColor);
                break;
            }
        }

        const float ElapsedTime = Span.GetTotalSeconds();
        const float Alpha = FMath::GetMappedRangeValueClamped(FVector2d(Config->Duration, Config->Duration + Config->FadeOut), FVector2d(1.0f, 0.0f), ElapsedTime);

        BackColor.w *= Alpha;
        TextColor.w *= Alpha;
        BorderColor.w *= Alpha;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, BackColor); 
        ImGui::PushStyleColor(ImGuiCol_Border, BorderColor); 
        ImGui::PushStyleColor(ImGuiCol_Text, TextColor);

        const auto Message = StringCast<ANSICHAR>(*Notification.Message);
        const float WrapWidth = Config->TextWrapping * DpiScale;

        ImGui::SetNextWindowViewport(Viewport->ID);
        ImGui::SetNextWindowPos(WindowPos, ImGuiCond_Always, FCogImguiHelper::ToImVec2(Config->Alignment));
        if (Config->UseFixedWidth)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(WrapWidth + WindowPadding.x * 2, 0), ImVec2(WrapWidth + WindowPadding.x * 2, MaxHeight));
        }
        
        if (ImGui::Begin(StringCast<ANSICHAR>(*Notification.Id).Get(), nullptr, Flags))
        {
            ImGui::PushTextWrapPos(WrapWidth);
            ImGui::TextUnformatted(Message.Get());
            ImGui::PopTextWrapPos();
        }

        ImGui::PopStyleColor(3);

        //----------------------------------------------------------------------
        // Compute ourself window height otherwise we get a one frame glitch,
        // maybe because the real window size is  computed the next frame.
        //----------------------------------------------------------------------
        const ImVec2 TextSize = ImGui::CalcTextSize(Message.Get(), nullptr, false, WrapWidth);
        const float WindowHeight = FMath::Min(MaxHeight, TextSize.y + (WindowPadding.y * 2));
        WindowPos.y += (WindowHeight + ItemSpacing.y) * (Config->Alignment.Y > 0.5f ? -1 : 1);
        ImGui::End();
    }

    ImGui::PopStyleVar(2);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderContent()
{
    Super::RenderContent();

    if (ImGui::Button("Clear Notifications", ImVec2(-1, 0)))
    {
        Notifications.Empty();
    }

    FCogWidgets::ThinSeparatorText("Notification Test");

    if (ImGui::Button("Notify Normal", ImVec2(-1, 0)))
    {
        COG_NOTIFY(TEXT("A notification test. Frame:%llu"), GFrameCounter);
    }
        
    if (ImGui::Button("Notify Warning", ImVec2(-1, 0)))
    {
        COG_NOTIFY_WARNING(TEXT("A long long long long long long long long long long long long long long long long long long long long long warning notification test. Frame:%llu"), GFrameCounter);
    }
        
    if (ImGui::Button("Notify Error", ImVec2(-1, 0)))
    {
        COG_NOTIFY_ERROR(TEXT("An error notification test. Frame:%llu"), GFrameCounter);
    }

    RenderSettings();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderSettings()
{
    FCogWidgets::ThinSeparatorText("Filtering");
    
    ImGui::Checkbox("Disable Notifications", &Config->DisableNotifications);

    ImGui::Checkbox("Notify Console Commands", &Config->NotifyConsoleCommands);
    
    ImGui::Checkbox("Notify All Warnings", &Config->NotifyAllWarnings);
    
    ImGui::Checkbox("Notify All Errors", &Config->NotifyAllErrors);

    FCogWidgets::ThinSeparatorText("Location & Size");

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat2("Alignment", &Config->Alignment.X, 0, 1.0f, "%.2f");
    
    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderInt2("Padding", &Config->Padding.X, 0, 100);
    
    ImGui::Checkbox("Use Fixed Width", &Config->UseFixedWidth);

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderInt("Text Wrapping", &Config->TextWrapping, 1, 500);

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderInt("Max Height", &Config->MaxHeight, 0, 500);

    FCogWidgets::ThinSeparatorText("Display");

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat("Duration", &Config->Duration, 1, 10, "%0.1f");

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat("Fade Out", &Config->FadeOut, 0, 3, "%0.1f");

    ImGui::Checkbox("Show Border", &Config->ShowBorder);
    
    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderInt("Rounding", &Config->Rounding, 0, 12);

    FCogWidgets::ThinSeparatorText("Colors");

    constexpr ImGuiColorEditFlags ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;

    FCogImguiHelper::ColorEdit4("##BackDef", Config->BackgroundDefaultColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Background Default");
    FCogImguiHelper::ColorEdit4("##BackWarn", Config->BackgroundWarningColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Background Warning");
    FCogImguiHelper::ColorEdit4("##BackError", Config->BackgroundErrorColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Background Error");
    ImGui::TextUnformatted("Background Color");
            
    FCogImguiHelper::ColorEdit4("##BorderDef", Config->BorderDefaultColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Border Default");
    FCogImguiHelper::ColorEdit4("##BorderWarn", Config->BorderWarningColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Border Warning");
    FCogImguiHelper::ColorEdit4("##BorderError", Config->BorderErrorColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Border Error");
    ImGui::TextUnformatted("Border Color");

    FCogImguiHelper::ColorEdit4("##TextDef", Config->TextDefaultColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Text Default");
    FCogImguiHelper::ColorEdit4("##TextWarn", Config->TextWarningColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Text Warning");
    FCogImguiHelper::ColorEdit4("##TextError", Config->TextErrorColor, ColorEditFlags);
    ImGui::SameLine();
    ImGui::SetItemTooltip("Text Error");
    ImGui::TextUnformatted("Text Color");
            
    ImGui::Separator();

    if (ImGui::Button("Reset Settings", ImVec2(-1, 0)))
    {
        ResetConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogNotificationOutputDevice
//--------------------------------------------------------------------------------------------------------------------------
FCogNotificationOutputDevice::FCogNotificationOutputDevice()
{
    GLog->AddOutputDevice(this);
}

//--------------------------------------------------------------------------------------------------------------------------
FCogNotificationOutputDevice::~FCogNotificationOutputDevice()
{
    if (GLog != nullptr)
    {
        GLog->RemoveOutputDevice(this);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogNotificationOutputDevice::Serialize(const TCHAR* Message, const ELogVerbosity::Type Verbosity, const FName& Category)
{
    if (Notifications != nullptr)
    {
        Notifications->OnLogReceived(Message, Verbosity, Category);
    }
}