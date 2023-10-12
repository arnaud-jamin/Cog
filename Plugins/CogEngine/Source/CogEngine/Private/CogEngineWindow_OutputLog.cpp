#include "CogEngineWindow_OutputLog.h"

#include "CogDebugHelper.h"
#include "Engine/Engine.h"
#include "Misc/StringBuilder.h"

char ImGuiTextBuffer::EmptyString[1] = { 0 };

//--------------------------------------------------------------------------------------------------------------------------
// FCogWindow_Log
//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_OutputLog::UCogEngineWindow_OutputLog()
{
    bHasMenu = true;
    OutputDevice.OutputLog = this;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_OutputLog::RenderHelp()
{
    ImGui::Text(
    "This window output the log based on each log categories verbosity. "
    "The verbosity of each log category can be configured in the 'Log Categories' window. "
    );
}
//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_OutputLog::ResetConfig()
{
    Super::ResetConfig();

    AutoScroll = true;
    ShowFrame = true;
    ShowCategory = true;
    ShowVerbosity = false;
    ShowAsTable = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_OutputLog::Clear()
{
    TextBuffer.clear();
    LineInfos.Empty();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_OutputLog::AddLog(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category)
{
    static TAnsiStringBuilder<512> Format;

    Format.Reset();

    if (Message)
    {
        Format.Append(Message);
    }

    FLineInfo& LineInfo = LineInfos.AddDefaulted_GetRef();
    LineInfo.Frame = GFrameCounter % 1000;
    LineInfo.Verbosity = Verbosity;
    LineInfo.Category = Category;
    LineInfo.Start = TextBuffer.size();

    TextBuffer.append(Format.GetData(), Format.GetData() + Format.Len());

    LineInfo.End = TextBuffer.size();

}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_OutputLog::DrawRow(const char* BufferStart, const FLineInfo& LineInfo, bool IsTableShown)
{
    ImU32 Color;
    switch (LineInfo.Verbosity)
    {
    case ELogVerbosity::Error:      Color = IM_COL32(255, 0, 0, 255); break;
    case ELogVerbosity::Warning:    Color = IM_COL32(255, 200, 0, 255); break;
    default:                        Color = IM_COL32(200, 200, 200, 255); break;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, Color);

    if (IsTableShown)
    {
        ImGui::TableNextRow();

        if (ShowFrame)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%3d", LineInfo.Frame);
        }

        if (ShowCategory)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(*LineInfo.Category.ToString()));
        }

        if (ShowVerbosity)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(ToString(LineInfo.Verbosity)));
        }

        ImGui::TableNextColumn();
        const char* LineStart = BufferStart + LineInfo.Start;
        const char* LineEnd = BufferStart + LineInfo.End;
        ImGui::TextUnformatted(LineStart, LineEnd);
    }
    else
    {
        if (ShowFrame)
        {
            ImGui::Text("[%3d] ", LineInfo.Frame);
            ImGui::SameLine();
        }

        if (ShowCategory)
        {
            ImGui::Text("%s: ", TCHAR_TO_ANSI(*LineInfo.Category.ToString()));
            ImGui::SameLine();
        }

        if (ShowVerbosity)
        {
            ImGui::Text("%s: ", TCHAR_TO_ANSI(ToString(LineInfo.Verbosity)));
            ImGui::SameLine();
        }

        const char* LineStart = BufferStart + LineInfo.Start;
        const char* LineEnd = BufferStart + LineInfo.End;
        ImGui::TextUnformatted(LineStart, LineEnd);
    }

    ImGui::PopStyleColor();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_OutputLog::RenderContent()
{
    Super::RenderContent();

    bool ClearPressed = false;
    bool CopyPressed = false;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Copy"))
            {
                ImGui::LogToClipboard();
            }
            ImGui::Separator();

            ImGui::Checkbox("Auto Scroll", &AutoScroll);
            ImGui::Checkbox("Show Frame", &ShowFrame);
            ImGui::Checkbox("Show Category", &ShowCategory);
            ImGui::Checkbox("Show Verbosity", &ShowVerbosity);
            ImGui::Checkbox("Show As Table", &ShowAsTable);

            ImGui::EndMenu();
        }

        ImGui::SameLine();

        if (ImGui::MenuItem("Clear"))
        {
            Clear();
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 9);
        if (ImGui::BeginCombo("##Verbosity", FCogDebugHelper::VerbosityToString((ELogVerbosity::Type)VerbosityFilter)))
        {
            for (int32 i = (int32)ELogVerbosity::Error; i <= (int32)ELogVerbosity::VeryVerbose; ++i)
            {
                bool IsSelected = i == VerbosityFilter;
                ELogVerbosity::Type Verbosity = (ELogVerbosity::Type)i;

                if (ImGui::Selectable(FCogDebugHelper::VerbosityToString(Verbosity), IsSelected))
                {
                    VerbosityFilter = i;
                }
            }
            ImGui::EndCombo();
        }

        FCogWindowWidgets::MenuSearchBar(Filter);

        ImGui::EndMenuBar();
    }

    int32 ColumnCount = 1;
    ColumnCount += (int32)ShowFrame;
    ColumnCount += (int32)ShowCategory;
    ColumnCount += (int32)ShowVerbosity;

    bool IsTableShown = false;
    if (ShowAsTable)
    {
        if (ImGui::BeginTable("LogTable", ColumnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ScrollX))
        {
            IsTableShown = true;
            if (ShowFrame)
            {
                ImGui::TableSetupColumn("Frame", ImGuiTableColumnFlags_WidthFixed, FCogWindowWidgets::GetFontWidth() * 4);
            }

            if (ShowCategory)
            {
                ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthFixed, FCogWindowWidgets::GetFontWidth() * 10);
            }

            if (ShowVerbosity)
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

    const char* BufferStart = TextBuffer.begin();

    if (Filter.IsActive())
    {
        for (int32 LineIndex = 0; LineIndex < LineInfos.Num(); LineIndex++)
        {
            const FLineInfo& LineInfo = LineInfos[LineIndex];
            const char* LineStart = BufferStart + LineInfo.Start;
            const char* LineEnd = BufferStart + LineInfo.End;
            if (Filter.PassFilter(LineStart, LineEnd))
            {
                DrawRow(BufferStart, LineInfo, IsTableShown);
            }
        }
    }
    else if (VerbosityFilter != ELogVerbosity::VeryVerbose)
    {
        for (int32 LineIndex = 0; LineIndex < LineInfos.Num(); LineIndex++)
        {
            const FLineInfo& LineInfo = LineInfos[LineIndex];

            if (LineInfo.Verbosity <= (ELogVerbosity::Type)VerbosityFilter)
            {
                const char* LineStart = BufferStart + LineInfo.Start;
                const char* LineEnd = BufferStart + LineInfo.End;
                DrawRow(BufferStart, LineInfo, IsTableShown);
            }
        }
    }
    else
    {
        ImGuiListClipper Clipper;
        Clipper.Begin(LineInfos.Num());
        while (Clipper.Step())
        {
            for (int32 LineIndex = Clipper.DisplayStart; LineIndex < Clipper.DisplayEnd; LineIndex++)
            {
                if (LineInfos.IsValidIndex(LineIndex))
                {
                    const FLineInfo& LineInfo = LineInfos[LineIndex];
                    DrawRow(BufferStart, LineInfo, IsTableShown);
                }
            }
        }
        Clipper.End();
    }

    if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
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
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogLogOutputDevice
//--------------------------------------------------------------------------------------------------------------------------
UCogLogOutputDevice::UCogLogOutputDevice()
{
    GLog->AddOutputDevice(this);
}

//--------------------------------------------------------------------------------------------------------------------------
UCogLogOutputDevice::~UCogLogOutputDevice()
{
    if (GLog != nullptr)
    {
        GLog->RemoveOutputDevice(this);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogLogOutputDevice::Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category)
{
    if (OutputLog != nullptr)
    {
        OutputLog->AddLog(Message, Verbosity, Category);
    }
}