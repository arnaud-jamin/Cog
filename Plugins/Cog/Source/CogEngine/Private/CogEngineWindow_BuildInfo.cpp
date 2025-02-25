#include "CogEngineWindow_BuildInfo.h"

#include "CogImguiHelper.h"
#include "imgui.h"
#include "BuildSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_BuildInfo::Initialize()
{
    FCogWindow::Initialize();
    
    Config = GetConfig<UCogEngineConfig_BuildInfo>();

    BuildText();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_BuildInfo::RenderHelp()
{
    ImGui::Text(
        "This window can be used to display the build information such as the build version, changelist, date, target, and so on."
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_BuildInfo::RenderTick(float DeltaTime)
{
    FCogWindow::RenderTick(DeltaTime);

    if (FApp::GetBuildTargetType() == EBuildTargetType::Editor)
    {
        if (Config->ShowInEditor == false)
        { return; }        
    }
    else
    {
        if (Config->ShowInPackage == false)
        { return;}
    }

    const auto TextStr = StringCast<ANSICHAR>(*Text);
    ImDrawList* DrawList = Config->ShowInForeground ? ImGui::GetForegroundDrawList() : ImGui::GetBackgroundDrawList();
    const ImVec2 WindowPadding = ImGui::GetStyle().WindowPadding;
    const ImVec2 TextSize = ImGui::CalcTextSize(TextStr.Get(), nullptr, false);
    const ImVec2 RectSize = TextSize + WindowPadding * 2;
    const ImVec2 Pos = FCogWidgets::ComputeScreenCornerLocation(Config->Alignment, Config->Padding);
    const ImVec2 AlignedPos = Pos - (FCogImguiHelper::ToImVec2(Config->Alignment) * RectSize);
    
    DrawList->AddRectFilled(AlignedPos, AlignedPos + RectSize, FCogImguiHelper::ToImU32(Config->BackgroundColor), Config->Rounding);
    DrawList->AddRect(AlignedPos, AlignedPos + RectSize, FCogImguiHelper::ToImU32(Config->BorderColor), Config->Rounding);
    DrawList->AddText(AlignedPos + WindowPadding, FCogImguiHelper::ToImU32(Config->TextColor), TextStr.Get());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_BuildInfo::RenderContent()
{
    Super::RenderContent();

    FCogWidgets::ThinSeparatorText("Build Properties");

    if (ImGui::BeginChild("Settings", ImVec2(-1, 100 * GetDpiScale()), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
    {
        if (ImGui::Checkbox("Branch Name", &Config->ShowBranchName)) { BuildText(); }
        if (ImGui::Checkbox("Build Date", &Config->ShowBuildDate)) { BuildText(); }
        if (ImGui::Checkbox("Build Configuration", &Config->ShowBuildConfiguration)) { BuildText(); }
        if (ImGui::Checkbox("Build User", &Config->ShowBuildUser)) { BuildText(); }
        if (ImGui::Checkbox("Build Machine", &Config->ShowBuildMachine)) { BuildText(); }
        if (ImGui::Checkbox("Build Target Type", &Config->ShowBuildTargetType)) { BuildText(); }  
        if (ImGui::Checkbox("Current Change list", &Config->ShowCurrentChangelist)) { BuildText(); }
        if (ImGui::Checkbox("Compatible Change list", &Config->ShowCompatibleChangelist)) { BuildText(); }
    }
    ImGui::EndChild();

    FCogWidgets::ThinSeparatorText("Display");

    ImGui::Checkbox("Show In Editor", &Config->ShowInEditor);
    ImGui::Checkbox("Show In Package", &Config->ShowInPackage);
    ImGui::Checkbox("Show In Foreground", &Config->ShowInForeground);

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderFloat2("Alignment", &Config->Alignment.X, 0, 1.0f, "%.2f");

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderInt2("Padding", &Config->Padding.X, 0, 100);

    FCogWidgets::SetNextItemToShortWidth();
    ImGui::SliderInt("Rounding", &Config->Rounding, 0, 12);

    if (FCogWidgets::InputText("Separator", Config->Separator))
    {
        BuildText();
    }

    constexpr ImGuiColorEditFlags ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
    FCogImguiHelper::ColorEdit4("Background Color", Config->BackgroundColor, ColorEditFlags);
    FCogImguiHelper::ColorEdit4("Border Color", Config->BorderColor, ColorEditFlags);
    FCogImguiHelper::ColorEdit4("Text Color", Config->TextColor, ColorEditFlags);

    ImGui::Separator();

    if (ImGui::Button("Reset Settings", ImVec2(-1, 0)))
    {
        ResetConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_BuildInfo::BuildText()
{
    FStringBuilderBase S;
    bool AddSeparator = false;

    if (Config->ShowBranchName) { S.Append(BuildSettings::GetBranchName()); S.Append(Config->Separator); }
    if (Config->ShowBuildDate) { S.Append(BuildSettings::GetBuildDate()); S.Append(Config->Separator); }
    if (Config->ShowBuildConfiguration) { S.Append(LexToString(FApp::GetBuildConfiguration())); S.Append(Config->Separator); }
    if (Config->ShowBuildTargetType) { S.Append(LexToString(FApp::GetBuildTargetType())); S.Append(Config->Separator); }
    if (Config->ShowBuildUser) { S.Append(BuildSettings::GetBuildUser()); S.Append(Config->Separator); }
    if (Config->ShowBuildMachine) { S.Append(BuildSettings::GetBuildMachine()); S.Append(Config->Separator); }
    if (Config->ShowCurrentChangelist) { S.Appendf(TEXT("%d"), BuildSettings::GetCurrentChangelist()); S.Append(Config->Separator); }
    if (Config->ShowCompatibleChangelist) { S.Appendf(TEXT("%d"),BuildSettings::GetCompatibleChangelist()); S.Append(Config->Separator); }

    S.RemoveSuffix(Config->Separator.Len());
    
    Text = FString(S);
} 