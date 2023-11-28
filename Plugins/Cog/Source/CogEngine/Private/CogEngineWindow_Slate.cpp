#include "CogEngineWindow_Slate.h"

#include "CogImguiHelper.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Slate::RenderHelp()
{
    ImGui::Text(
        "This window displays slate debug info. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Slate::RenderContent()
{
    Super::RenderContent();

    
    if (FSlateApplication::IsInitialized() == false)
    {
        ImGui::Text("Not initialized");
        return;
    }

    FSlateApplication& SlateApp = FSlateApplication::Get();

    static int32 SelectedUserIndex = 0;

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##User", TCHAR_TO_ANSI(*FString::Printf(TEXT("%d"), SelectedUserIndex))))
    {
        SlateApp.ForEachUser([this](FSlateUser& User)
        {
            if (ImGui::Selectable(TCHAR_TO_ANSI(*FString::Printf(TEXT("%d"), SelectedUserIndex)), false))
            {
                SelectedUserIndex = User.GetUserIndex();
            }
        });
        ImGui::EndCombo();
    }

    if (TSharedPtr<FSlateUser> User = SlateApp.GetUser(SelectedUserIndex))
    {
        RenderUser(*User.Get());
    }

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Slate::RenderUser(FSlateUser& User)
{

    if (ImGui::BeginTable("SlateUser", 2, ImGuiTableFlags_Borders))
    {
        const ImVec4 LabelColor(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");


        //------------------------
        // Focused Widget
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(LabelColor, "Focused Widget");
        ImGui::TableNextColumn();
        FString FocusedWidgetText = "None";
        if (TSharedPtr<SWidget> FocusedWidget = User.GetFocusedWidget())
        {
            FocusedWidgetText = FocusedWidget->ToString();
        }
        ImGui::Text("%s", TCHAR_TO_ANSI(*FocusedWidgetText));

        //------------------------
        // Cursor Captor
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(LabelColor, "Cursor Captor");
        ImGui::TableNextColumn();
        FString CursorCaptoreWidgetText = "None";
        if (TSharedPtr<SWidget> CursorCaptoreWidget = User.GetCursorCaptor())
        {
            CursorCaptoreWidgetText = CursorCaptoreWidget->ToString();
        }
        ImGui::Text("%s", TCHAR_TO_ANSI(*CursorCaptoreWidgetText));

        //------------------------
        // Cursor Position
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(LabelColor, "Cursor Position");
        ImGui::TableNextColumn();
        ImVec2 CursorPosition = FCogImguiHelper::ToImVec2(User.GetCursorPosition());
        ImGui::InputFloat2("##Cursor Position", &CursorPosition.x, "%0.0f");



        ImGui::EndTable();
    }
}