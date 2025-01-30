#include "CogEngineWindow_Console.h"

#include <ranges>

#include "CogImguiHelper.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "GameFramework/PlayerController.h"
#include "imgui.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::RenderHelp()
{
    ImGui::Text("This window can be used as a replacement of the Unreal console.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::Initialize()
{
    FCogWindow::Initialize();

    Config = GetConfig<UCogEngineConfig_Console>();

    bNoPadding = true;
    bHasMenu = true;
    bHasWidget = 1;
    
    RefreshVisibleCommands();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::RenderContent()
{
    Super::RenderContent();

    const APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::Checkbox("Sort Commands", &Config->SortCommands))
            {
                RefreshVisibleCommands();
            }

            if (ImGui::Checkbox("Show Console Input In Menu Bar", &Config->ShowConsoleInputInMenuBar))
            {
                RefreshVisibleCommands();
            }
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            if (ImGui::SliderInt("Num History Commands", &Config->NumHistoryCommands, 0, 100))
            {
                RefreshVisibleCommands();
            }

            FCogWindowWidgets::SetNextItemToShortWidth();
            if (ImGui::SliderInt("Completion Minimum Characters", &Config->CompletionMinimumCharacters, 0, 3))
            {
                RefreshVisibleCommands();
            }

            ImGui::ColorEdit4("History Color", (float*)&Config->HistoryColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            
            ImGui::EndMenu();
        }

        if (Config->ShowConsoleInputInMenuBar)
        {
            ImGui::SetNextItemWidth(-1);
            RenderConsoleTextInput();
        }
        
        ImGui::EndMenuBar();
    }

    ImGui::Spacing();
    
    if (Config->ShowConsoleInputInMenuBar == false)
    {
        ImGui::SetNextItemWidth(-1);
        RenderConsoleTextInput();
    }
    
    const float Indent = ImGui::GetFontSize() * 0.5f;
    int32 Index = 0;

    if (ImGui::BeginChild("Commands", ImVec2(0.0f, -1.0f), ImGuiChildFlags_NavFlattened))
    {
        ImGui::Indent(Indent);

        for (const FString& CommandName : VisibleHistory)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(Config->HistoryColor));
            ImGui::PushID(Index);
            RenderCommand(CommandName, Index);
            ImGui::PopID();
            ImGui::PopStyleColor();

            Index++;
        }

        if (Index > 1)
        {
            ImGui::Separator();
        }

        ImGuiListClipper Clipper;
        Clipper.Begin(VisibleCommands.Num());

        // int32 ClipperIndex = 0;
        // while (Clipper.Step())
        // {
        //     for (ClipperIndex = Clipper.DisplayStart; ClipperIndex < Clipper.DisplayEnd; ClipperIndex++)
        //     {
        //         if (VisibleCommands.IsValidIndex(ClipperIndex))
        //         {
        //             ImGui::PushID(Index);
        //             const FString& CommandName = VisibleCommands[ClipperIndex];
        //             RenderCommand(CommandName, Index);
        //             if (SelectedCommandIndex == Index)
        //             {
        //                 SelectedCommand = CommandName;
        //             }
        //             ImGui::PopID();
        //             Index++;
        //         }
        //     }
        // }
        //
        // Clipper.End();
        //
        // // If any is available, draw an additional command below the clipper to be able to scroll when pressing bottom
        // if (VisibleCommands.IsValidIndex(ClipperIndex + 1))
        // {
        //     const FString& Command = VisibleCommands[ClipperIndex + 1];
        //     RenderCommand(Command, Index);
        //     Index++;
        // }
        
        for (const FString& CommandName : VisibleCommands)
        {
            RenderCommand(CommandName, Index);
            Index++;
        }
        
        ImGui::Unindent(Indent);
    }
    ImGui::EndChild();

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
    {
        if (bIsWindowFocused == false)
        {
            bRequestTextInputFocus = true;
        }
        bIsWindowFocused = true;
        
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            SelectedCommandIndex += 1;
            bScroll = true;

            if (SelectedCommandIndex >= Index)
            {
                SelectedCommandIndex = 0;
            }
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            SelectedCommandIndex -= 1;
            bScroll = true;

            if (SelectedCommandIndex < 0)
            {
                SelectedCommandIndex = Index - 1;
            }
        }
    }
    else
    {
        bIsWindowFocused = false;
    }
    
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
    // if (ImGui::BeginChild("Description", ImVec2(0.0f, ImGui::GetContentRegionAvail().y)))
    // {
    //     ImGui::Indent(Indent);
    //     ImGui::BeginDisabled();
    //
    //     const FString Help = GetConsoleCommandHelp(CurrentUserInput);
    //     const auto& HelpStr = StringCast<ANSICHAR>(*Help);
    //     ImGui::TextWrapped(HelpStr.Get());
    //
    //     ImGui::EndDisabled();
    //     ImGui::Unindent(Indent);
    // }
    // ImGui::EndChild();
    // ImGui::PopStyleColor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::RenderConsoleTextInput()
{
    constexpr ImGuiInputTextFlags InputFlags =
        ImGuiInputTextFlags_EnterReturnsTrue
        | ImGuiInputTextFlags_EscapeClearsAll
        | ImGuiInputTextFlags_CallbackCompletion
        | ImGuiInputTextFlags_CallbackEdit;
    
    if (FCogWindowWidgets::InputTextWithHint("##Command", "Command", CurrentUserInput, InputFlags, &OnTextInputCallbackStub, this))
    {
        ExecuteCommand(CurrentUserInput);
        bRequestTextInputFocus = true;
    }

    ImGui::SetItemDefaultFocus();
    
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
        && ImGui::IsItemActive() == false
        && bRequestTextInputFocus
        && IsWindowRenderedInMainMenu() == false)
    {
        ImGui::SetKeyboardFocusHere(-1);
    }

    if (ImGui::IsItemActive())
    {
        bRequestTextInputFocus = false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogEngineWindow_Console::GetConsoleCommandHelp(const FString& InCommandName)
{
    TArray<FString> CommandSplitWithSpaces;
    InCommandName.ParseIntoArrayWS(CommandSplitWithSpaces);

    if (CommandSplitWithSpaces.Num() > 0)
    {
        if (const IConsoleObject* ConsoleObject = IConsoleManager::Get().FindConsoleObject(*CommandSplitWithSpaces[0]))
        {
            return FString(ConsoleObject->GetHelp());
        }
    }
    
    return FString("Unknown command.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::RenderCommand(const FString& CommandName, const int32 Index)
{
    const auto& CommandNameStr = StringCast<ANSICHAR>(*CommandName);

    bool IsSelected = Index == SelectedCommandIndex;

    // ImGui::Text("%d - ", Index);
    // ImGui::SameLine();
    
    if (ImGui::Selectable(CommandNameStr.Get(), &IsSelected, ImGuiSelectableFlags_AllowDoubleClick))
    {
        SelectedCommandIndex = Index;
        CurrentUserInput = CommandName;
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            ExecuteCommand(CommandName);
        }
    }

    if (IsSelected)
    {
        SelectedCommand = CommandName;
        if (bScroll)
        {
            ImGui::SetScrollHereY(1.0f);
            bScroll = false;
        }
    }

    if (ImGui::BeginItemTooltip())
    {
        const FString Help = GetConsoleCommandHelp(CommandName);
        const auto& HelpStr = StringCast<ANSICHAR>(*Help);

        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(HelpStr.Get());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::RefreshVisibleCommands()
{
    auto OnConsoleObject = [this](const TCHAR *InName, const IConsoleObject* InConsoleObject)
    {
        if (InConsoleObject->TestFlags(ECVF_Unregistered) || InConsoleObject->TestFlags(ECVF_ReadOnly))
        { return; }

        VisibleCommands.Add(InName);
    };

    FString CommandStart = CurrentUserInput;

    //------------------------------------------------------------------------------------------------------
    // Split the user input with spaces tp get the first part of the command so the completion is made on
    // "Cog.Cheat" instead of "Cog.Cheat God", as the later would return no results 
    //------------------------------------------------------------------------------------------------------
    TArray<FString> UserInputSplitWithSpaces;
    CurrentUserInput.ParseIntoArrayWS(UserInputSplitWithSpaces);
    if (UserInputSplitWithSpaces.Num() > 0)
    {
        CommandStart = UserInputSplitWithSpaces[0];
    }
        
    VisibleCommands.Empty();
    if (CommandStart.Len() >= Config->CompletionMinimumCharacters)
    {
        IConsoleManager::Get().ForEachConsoleObjectThatStartsWith(FConsoleObjectVisitor::CreateLambda(OnConsoleObject), *CommandStart);
    }

    if (Config->SortCommands)
    {
        VisibleCommands.Sort();
    }

    TArray<FString> AllHistory;
    IConsoleManager::Get().GetConsoleHistory(TEXT(""), AllHistory);

    VisibleHistory.Empty();
    for (int32 i = AllHistory.Num() - 1; i >= 0; i--)
    {
        FString Command = AllHistory[i];
        if (Command.IsEmpty())
        { continue; }

        if (CurrentUserInput.IsEmpty() == false && Command.StartsWith(CurrentUserInput) == false)
        { continue; }

        if (VisibleHistory.Num() >= Config->NumHistoryCommands)
        { break; }

        VisibleHistory.Add(Command);
    }

    SelectedCommandIndex = 0;
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogEngineWindow_Console::OnTextInputCallback(ImGuiInputTextCallbackData* InData)
{
    if (InData->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        FString CleanupCommand = SelectedCommand.TrimEnd(); 
        const auto& CommandStr = StringCast<ANSICHAR>(*CleanupCommand);
        InData->DeleteChars(0, InData->BufTextLen);
        InData->InsertChars(0, CommandStr.Get());
        InData->InsertChars(InData->CursorPos, " ");
    }
    else  if (InData->EventFlag == ImGuiInputTextFlags_CallbackEdit)
    {
        CurrentUserInput = FString(InData->Buf);
        RefreshVisibleCommands();        
    }

    return 0;
}

//--------------------------------------------------------------------------------------------------------------------------
int FCogEngineWindow_Console::OnTextInputCallbackStub(ImGuiInputTextCallbackData* InData)
{
    FCogEngineWindow_Console& ConsoleWindow	= *static_cast<FCogEngineWindow_Console*>(InData->UserData);
    return ConsoleWindow.OnTextInputCallback(InData);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::ExecuteCommand(const FString& InCommand)
{
    FString CleanupCommand = InCommand.TrimEnd(); 
    if (CleanupCommand.IsEmpty() == false)
    {
        if (VisibleHistory.Num() == 0 || (VisibleHistory.Last() != CleanupCommand))
        {
            IConsoleManager::Get().AddConsoleHistoryEntry(TEXT(""), *CleanupCommand);
        }
    
        GEngine->DeferredCommands.Add(CleanupCommand);
    }    

    CurrentUserInput = FString();
    RefreshVisibleCommands();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Console::RenderMainMenuWidget()
{
    ImGui::SetNextItemWidth(100);
    RenderConsoleTextInput();
}
    
