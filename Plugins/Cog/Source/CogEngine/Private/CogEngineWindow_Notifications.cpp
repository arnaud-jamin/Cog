#include "CogEngineWindow_Notifications.h"

#include "CogCommon.h"
#include "CogCommonLogCategory.h"
#include "CogDebugHelper.h"
#include "CogImguiHelper.h"
#include "CogWindowWidgets.h"
#include "imgui_internal.h"
#include "Engine/Engine.h"
#include "GenericPlatform/GenericPlatformSurvey.h"
#include "Math/UnitConversion.h"
#include "Misc/StringBuilder.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::Initialize()
{    
    Super::Initialize();

    bHasMenu = true;
    OutputDevice.Notifications = this;

    Config = GetConfig<UCogEngineConfig_Notifications>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderHelp()
{
    ImGui::Text(
    "This window output the log based on each log categories verbosity. "
    "The verbosity of each log category can be configured in the 'Log Categories' window. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::Clear()
{
    Notifications.Empty();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::AddLog(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity, const class FName& InCategory)
{
    if (InCategory == LogCogNotify.GetCategoryName())
    {
        FNotification& Notification = Notifications.AddDefaulted_GetRef();
        Notification.Frame = GFrameCounter;
        Notification.Time = FDateTime::Now();
        Notification.Verbosity = InVerbosity;
        Notification.Category = InCategory;
        Notification.Message = InMessage;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::DrawRow(const FNotification& InNotification, bool InShowAsTableRow) const
{
    ImU32 Color;
    switch (InNotification.Verbosity)
    {
        case ELogVerbosity::Error:      Color = FCogImguiHelper::ToImColor(Config->TextErrorColor); break;
        case ELogVerbosity::Warning:    Color = FCogImguiHelper::ToImColor(Config->TextWarningColor); break;
        default:                        Color = FCogImguiHelper::ToImColor(Config->TextDefaultColor); break;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, Color);

    if (InShowAsTableRow)
    {
        ImGui::TableNextRow();
    }
    
    if (Config->ShowVerbosity)
    {
        if (InShowAsTableRow)
        {
            ImGui::TableNextColumn();
        }

        ImGui::TextUnformatted(StringCast<ANSICHAR>(ToString(InNotification.Verbosity)).Get());
        
        if (InShowAsTableRow == false)
        {
            ImGui::SameLine();
        }
    }

    if (InShowAsTableRow)
    {
        ImGui::TableNextColumn();
    }
    
    ImGui::TextUnformatted(StringCast<ANSICHAR>(*InNotification.Message).Get());

    ImGui::PopStyleColor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

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
        | ImGuiWindowFlags_NoSavedSettings;
    
    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    const ImVec2 ViewportPos = Viewport->WorkPos;
    const ImVec2 ViewportSize = Viewport->WorkSize;

    const bool IsRight = static_cast<int32>(Config->Location) & 1;
    const bool IsBottom = static_cast<int32>(Config->Location) & 2;
    
    ImVec2 WindowPos;
    WindowPos.x = ViewportPos.x + (IsRight ? ViewportSize.x - Config->Padding : Config->Padding);
    WindowPos.y = ViewportPos.y + (IsBottom ? ViewportSize.y - Config->Padding : Config->Padding);

    const ImVec2 WindowPosPivot(IsRight ? 1.0f : 0.0f, IsBottom ? 1.0f : 0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Config->WindowRounding); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Config->WindowBorder); 

    for (int32 i = Notifications.Num() - 1; i >= 0; i--)
    {
        const FNotification& Notification = Notifications[i];

        const FTimespan Span = FDateTime::Now() - Notification.Time;
        if (Span > FTimespan::FromSeconds(Config->VisibleDuration))
        {
            Notifications.RemoveAt(i);
            continue;
        }
        
        ImU32 TextColor, BackColor, BorderColor;
        switch (Notification.Verbosity)
        {
            case ELogVerbosity::Error:
            {
                TextColor = FCogImguiHelper::ToImColor(Config->TextErrorColor);
                BackColor = FCogImguiHelper::ToImColor(Config->BackgroundErrorColor);
                BorderColor = FCogImguiHelper::ToImColor(Config->BorderErrorColor);
                break;
            }
                
            case ELogVerbosity::Warning:
            {
                TextColor = FCogImguiHelper::ToImColor(Config->TextWarningColor); 
                BackColor = FCogImguiHelper::ToImColor(Config->BackgroundWarningColor);
                BorderColor = FCogImguiHelper::ToImColor(Config->BorderWarningColor);
                break;
            }
                
            default:
            {
                TextColor = FCogImguiHelper::ToImColor(Config->TextDefaultColor);
                BackColor = FCogImguiHelper::ToImColor(Config->BackgroundDefaultColor);
                BorderColor = FCogImguiHelper::ToImColor(Config->BorderDefaultColor);
                break;
            }
        }
        
        ImGui::SetNextWindowPos(WindowPos, ImGuiCond_Always, WindowPosPivot);
        ImGui::SetNextWindowViewport(Viewport->ID);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, BackColor); 
        ImGui::PushStyleColor(ImGuiCol_Border, BorderColor); 
        ImGui::PushStyleColor(ImGuiCol_Text, TextColor);

        static char WindowNameBuffer[32];
        ImFormatString(WindowNameBuffer, IM_ARRAYSIZE(WindowNameBuffer), "CogNotification%d", i);
        if (ImGui::Begin(WindowNameBuffer, nullptr, Flags))
        {
            ImGui::PushTextWrapPos(Config->TextWrapping * ImGui::GetFontSize());
            ImGui::TextUnformatted(StringCast<ANSICHAR>(*Notification.Message).Get());
            ImGui::PopTextWrapPos();
        }

        ImGui::PopStyleColor(3);
        
        WindowPos.y += (ImGui::GetWindowHeight() + ImGui::GetStyle().ItemSpacing.y) * (IsBottom ? -1 : 1);
        ImGui::End();
    }

    ImGui::PopStyleVar(2);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Notifications::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Auto Scroll", &Config->AutoScroll);
            ImGui::Checkbox("Show Verbosity", &Config->ShowVerbosity);
            ImGui::Checkbox("Show As Table", &Config->ShowAsTable);
            ImGui::Checkbox("Window Border", &Config->WindowBorder);
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogWindowWidgets::ComboboxEnum("Notification Location", Config->Location);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Visible Duration", &Config->VisibleDuration, 1, 10, "%0.1f");

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Fade Duration", &Config->FadeDuration, 0, 3, "%0.1f");
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::DragInt("Text Wrapping", &Config->TextWrapping, 1, 0, INT_MAX);
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::DragInt("Padding", &Config->Padding, 1, 0, INT_MAX);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderInt("Window Rounding", &Config->WindowRounding, 0, 12);

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
            
            ImGui::EndMenu();
        }

        ImGui::SameLine();

        if (ImGui::MenuItem("Clear"))
        {
            Clear();
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 9);
        if (ImGui::BeginCombo("##Verbosity", FCogDebugHelper::VerbosityToString(static_cast<ELogVerbosity::Type>(Config->VerbosityFilter))))
        {
            for (int32 i = ELogVerbosity::Error; i <= static_cast<int32>(ELogVerbosity::VeryVerbose); ++i)
            {
	            const bool IsSelected = i == Config->VerbosityFilter;
	            const ELogVerbosity::Type Verbosity = static_cast<ELogVerbosity::Type>(i);

                if (ImGui::Selectable(FCogDebugHelper::VerbosityToString(Verbosity), IsSelected))
                {
                    Config->VerbosityFilter = i;
                }
            }
            ImGui::EndCombo();
        }

        FCogWindowWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }

    int32 ColumnCount = 1;
    ColumnCount += static_cast<int32>(Config->ShowVerbosity);

    bool IsTableShown = false;
    if (Config->ShowAsTable)
    {
        if (ImGui::BeginTable("LogTable", ColumnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoHostExtendX))
        {
            IsTableShown = true;
            if (Config->ShowVerbosity)
            {
                ImGui::TableSetupColumn("Verbosity", ImGuiTableColumnFlags_WidthFixed, FCogWindowWidgets::GetFontWidth() * 10);
            }

            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
        }
    }

    if (IsTableShown == false)
    {
        ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    }

    if (Filter.IsActive())
    {
        for (int32 i = 0; i < Notifications.Num(); i++)
        {
            const FNotification& Notification = Notifications[i];
            const auto& Message = StringCast<ANSICHAR>(*Notification.Message);
            if (Filter.PassFilter(Message.Get()))
            {
                DrawRow(Notification, IsTableShown);
            }
        }
    }
    else if (Config->VerbosityFilter != ELogVerbosity::VeryVerbose)
    {
        for (int32 i = 0; i < Notifications.Num(); i++)
        {
            const FNotification& Notification = Notifications[i];

            if (Notification.Verbosity <= static_cast<ELogVerbosity::Type>(Config->VerbosityFilter))
            {
                DrawRow(Notification, IsTableShown);
            }
        }
    }
    else
    {
        ImGuiListClipper Clipper;
        Clipper.Begin(Notifications.Num());
        while (Clipper.Step())
        {
            for (int32 LineIndex = Clipper.DisplayStart; LineIndex < Clipper.DisplayEnd; LineIndex++)
            {
                if (Notifications.IsValidIndex(LineIndex))
                {
                    const FNotification& Notification = Notifications[LineIndex];
                    DrawRow(Notification, IsTableShown);
                }
            }
        }
        Clipper.End();
    }

    if (Config->AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    if (IsTableShown)
    {
        ImGui::EndTable();
    }
    else
    {
        ImGui::EndChild();
    }

    if (ImGui::BeginPopupContextItem("Notifications"))
    {
        if (ImGui::MenuItem("Clear"))
        {
            Clear();
        }

        ImGui::EndPopup();
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
        Notifications->AddLog(Message, Verbosity, Category);
    }
}