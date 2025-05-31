#include "CogEngineWindow_OutputLog.h"

#include "CogCommon.h"
#include "CogCommonLogCategory.h"
#include "CogDebugHelper.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/StringBuilder.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::Initialize()
{    
    Super::Initialize();

    bHasMenu = true;
    OutputDevice.OutputLog = this;

    Config = GetConfig<UCogEngineConfig_OutputLog>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::RenderHelp()
{
    ImGui::Text(
    "This window output the log based on each log categories verbosity. "
    "The verbosity of each log category can be configured in the 'Log Categories' window. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::Clear()
{
    TextBuffer.clear();
    LogInfos.Empty();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::AddLog(const TCHAR* InMessage, ELogVerbosity::Type InVerbosity, const class FName& InCategory)
{
    static TAnsiStringBuilder<512> Format;

    Format.Reset();

    if (InMessage)
    {
        Format.Append(InMessage);
    }

    FLogInfo& LogInfo = LogInfos.AddDefaulted_GetRef();
    LogInfo.Frame = GFrameCounter;
    LogInfo.Time = Config != nullptr && Config->UseUTCTime ? FDateTime::UtcNow() : FDateTime::Now();
    LogInfo.Verbosity = InVerbosity;
    LogInfo.Category = InCategory;
    LogInfo.LineStart = TextBuffer.size();

    TextBuffer.append(Format.GetData(), Format.GetData() + Format.Len());

    LogInfo.LineEnd = TextBuffer.size();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::DrawRow(const char* InBufferStart, const FLogInfo& InLogInfo, bool InShowAsTableRow) const
{
    ImU32 Color;
    switch (InLogInfo.Verbosity)
    {
        case ELogVerbosity::Error:      Color = FCogImguiHelper::ToImColor(Config->ErrorColor); break;
        case ELogVerbosity::Warning:    Color = FCogImguiHelper::ToImColor(Config->WarningColor); break;
        default:                        Color = FCogImguiHelper::ToImColor(Config->DefaultColor); break;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, Color);

    if (InShowAsTableRow)
    {
        ImGui::TableNextRow();
    }
    
    if (Config->ShowFrame)
    {
        if (InShowAsTableRow)
        {
            ImGui::TableNextColumn();
        }
        
        ImGui::Text("%3d", GetDisplayedFrame(InLogInfo));

        if (InShowAsTableRow == false)
        {
            ImGui::SameLine();
        }
    }

    if (Config->ShowTime)
    {
        if (InShowAsTableRow)
        {
            ImGui::TableNextColumn();
        }

        ImGui::TextUnformatted(StringCast<ANSICHAR>(*InLogInfo.Time.ToString()).Get());

        if (InShowAsTableRow == false)
        {
            ImGui::SameLine();
        }
    }

    if (Config->ShowCategory)
    {
        if (InShowAsTableRow)
        {
            ImGui::TableNextColumn();
        }

        ImGui::TextUnformatted(StringCast<ANSICHAR>(*InLogInfo.Category.ToString()).Get());

        if (InShowAsTableRow == false)
        {
            ImGui::SameLine();
        }
    }

    if (Config->ShowVerbosity)
    {
        if (InShowAsTableRow)
        {
            ImGui::TableNextColumn();
        }

        ImGui::TextUnformatted(StringCast<ANSICHAR>(ToString(InLogInfo.Verbosity)).Get());

        if (InShowAsTableRow == false)
        {
            ImGui::SameLine();
        }
    }

    if (InShowAsTableRow)
    {
        ImGui::TableNextColumn();
    }
    
    const char* LineStart = InBufferStart + InLogInfo.LineStart;
    const char* LineEnd = InBufferStart + InLogInfo.LineEnd;
    ImGui::TextUnformatted(LineStart, LineEnd);

    ImGui::PopStyleColor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::Copy() const
{
    const auto Buffer = StringCast<TCHAR>(TextBuffer.c_str());
    const auto BufferData = Buffer.Get();
    if (BufferData == nullptr)
    { return; }

    FStringBuilderBase StringBuilder;
    for (const FLogInfo& LogInfo : LogInfos)
    {
        StringBuilder.Append(FString::Printf(TEXT("[%3d] [%s] [%s] "), GetDisplayedFrame(LogInfo), *LogInfo.Category.ToString(), ToString(LogInfo.Verbosity)));
        StringBuilder.Append(BufferData + LogInfo.LineStart, LogInfo.LineEnd - LogInfo.LineStart);
        StringBuilder.Append("\n");
    };
    
    FPlatformApplicationMisc::ClipboardCopy(StringBuilder.ToString());
}

//--------------------------------------------------------------------------------------------------------------------------
int32 FCogEngineWindow_OutputLog::GetDisplayedFrame(const FCogEngineWindow_OutputLog::FLogInfo& InLogInfo) const
{
    return Config->FrameCycle > 0 ? InLogInfo.Frame % Config->FrameCycle : InLogInfo.Frame;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_OutputLog::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Copy"))
            {
                Copy();
            }
            ImGui::Separator();

            ImGui::Checkbox("Auto Scroll", &Config->AutoScroll);
            ImGui::Checkbox("Show Frame", &Config->ShowFrame);
            ImGui::Checkbox("Show Time", &Config->ShowTime);
            ImGui::Checkbox("Show Category", &Config->ShowCategory);
            ImGui::Checkbox("Show Verbosity", &Config->ShowVerbosity);
            ImGui::Checkbox("Show As Table", &Config->ShowAsTable);

            ImGui::Separator();
            
            constexpr ImGuiColorEditFlags ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
            FCogImguiHelper::ColorEdit4("Default Color", Config->DefaultColor, ColorEditFlags);
            FCogImguiHelper::ColorEdit4("Warning Color", Config->WarningColor, ColorEditFlags);
            FCogImguiHelper::ColorEdit4("Error Color", Config->ErrorColor, ColorEditFlags);

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

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }

    int32 ColumnCount = 1;
    ColumnCount += Config->ShowFrame ? 1 : 0;
    ColumnCount += Config->ShowTime ? 1 : 0;
    ColumnCount += Config->ShowCategory ? 1 : 0;
    ColumnCount += Config->ShowVerbosity ? 1 : 0;

    bool IsTableShown = false;
    if (Config->ShowAsTable)
    {
        if (ImGui::BeginTable("LogTable", ColumnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoHostExtendX))
        {
            IsTableShown = true;
            if (Config->ShowFrame)
            {
                ImGui::TableSetupColumn("Frame", ImGuiTableColumnFlags_WidthFixed, FCogWidgets::GetFontWidth() * 4);
            }

            if (Config->ShowCategory)
            {
                ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthFixed, FCogWidgets::GetFontWidth() * 10);
            }

            if (Config->ShowVerbosity)
            {
                ImGui::TableSetupColumn("Verbosity", ImGuiTableColumnFlags_WidthFixed, FCogWidgets::GetFontWidth() * 10);
            }

            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
        }
    }

    if (IsTableShown == false)
    {
        ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    }

    const char* BufferStart = TextBuffer.begin();

    if (Filter.IsActive())
    {
        for (int32 LineIndex = 0; LineIndex < LogInfos.Num(); LineIndex++)
        {
            const FLogInfo& LineInfo = LogInfos[LineIndex];
            const char* LineStart = BufferStart + LineInfo.LineStart;
            const char* LineEnd = BufferStart + LineInfo.LineEnd;
            if (Filter.PassFilter(LineStart, LineEnd))
            {
                DrawRow(BufferStart, LineInfo, IsTableShown);
            }
        }
    }
    else if (Config->VerbosityFilter != ELogVerbosity::VeryVerbose)
    {
        for (int32 LineIndex = 0; LineIndex < LogInfos.Num(); LineIndex++)
        {
            const FLogInfo& LineInfo = LogInfos[LineIndex];

            if (LineInfo.Verbosity <= static_cast<ELogVerbosity::Type>(Config->VerbosityFilter))
            {
                DrawRow(BufferStart, LineInfo, IsTableShown);
            }
        }
    }
    else
    {
        ImGuiListClipper Clipper;
        Clipper.Begin(LogInfos.Num());
        while (Clipper.Step())
        {
            for (int32 LineIndex = Clipper.DisplayStart; LineIndex < Clipper.DisplayEnd; LineIndex++)
            {
                if (LogInfos.IsValidIndex(LineIndex))
                {
                    const FLogInfo& LineInfo = LogInfos[LineIndex];
                    DrawRow(BufferStart, LineInfo, IsTableShown);
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

    if (ImGui::BeginPopupContextItem("OutputLog"))
    {
        if (ImGui::MenuItem("Clear"))
        {
            Clear();
        }

        if (ImGui::MenuItem("Copy"))
        {
            Copy();
        }

        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogLogOutputDevice
//--------------------------------------------------------------------------------------------------------------------------
FCogLogOutputDevice::FCogLogOutputDevice()
{
    GLog->AddOutputDevice(this);
}

//--------------------------------------------------------------------------------------------------------------------------
FCogLogOutputDevice::~FCogLogOutputDevice()
{
    if (GLog != nullptr)
    {
        GLog->RemoveOutputDevice(this);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogLogOutputDevice::Serialize(const TCHAR* Message, const ELogVerbosity::Type Verbosity, const FName& Category)
{
    if (OutputLog != nullptr)
    {
        OutputLog->AddLog(Message, Verbosity, Category);
    }
}