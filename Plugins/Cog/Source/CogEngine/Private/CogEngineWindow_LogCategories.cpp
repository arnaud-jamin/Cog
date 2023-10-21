#include "CogEngineWindow_LogCategories.h"

#include "CogDebugHelper.h"
#include "CogDebugSettings.h"
#include "CogWindowWidgets.h"
#include "CogDebugLog.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_LogCategories::RenderHelp()
{
    ImGui::Text(
        "This window can be used to activate and deactivate log categories."
        "Activating a log category set its verbosity to VeryVerbose. "
        "Deactivating a log category set its verbosity to Warning. "
        "The detailed verbosity of each log category can shown by using the Option menu. "
        "On a client, both the client and the server verbosity can be modified. "
        "The log categories are used to display both output log and debug display in the world. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_LogCategories::UCogEngineWindow_LogCategories()
{
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_LogCategories::ResetConfig()
{
    Super::ResetConfig();

    FCogDebugLog::DeactivateAllLogCateories(*GetWorld());
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_LogCategories::RenderContent()
{
    Super::RenderContent();

    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return;
    }

    static bool bShowAllVerbosity = false;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Deactivate All"))
            {
                FCogDebugLog::DeactivateAllLogCateories(*World);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
            {
                ImGui::SetTooltip("Deactivate all log categories. If connected to a server, also deactivate all its log categories.");
            }

            ImGui::Separator();

            ImGui::Checkbox("Show detailed verbosity", &bShowAllVerbosity);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
            {
                ImGui::SetTooltip("Show the verbosity level of each log category.");
            }

            ImGui::EndMenu();
        }

        bool bIsFilteringBySelection = FCogDebugSettings::GetIsFilteringBySelection();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
        FCogWindowWidgets::PushStyleCompact();
        if (ImGui::Checkbox("Filter", &bIsFilteringBySelection))
        {
            FCogDebugSettings::SetIsFilteringBySelection(GetWorld(), bIsFilteringBySelection);
        }
        FCogWindowWidgets::PopStyleCompact();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::SetTooltip("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");
        }

        if (ImGui::MenuItem("Flush"))
        {
            FlushPersistentDebugLines(World);
            FlushDebugStrings(GWorld);
            GEngine->ClearOnScreenDebugMessages();
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::SetTooltip("Clear all the debug drawn on screen");
        }

        ImGui::EndMenuBar();
    }

    const bool IsClient = World->GetNetMode() == NM_Client;

    ImGuiStyle& Style = ImGui::GetStyle();

    int Index = 0;
    for (const auto& Entry : FCogDebugLog::GetLogCategories())
    {
        FName CategoryName = Entry.Key;
        const FCogDebugLogCategoryInfo& CategoryInfo = Entry.Value;
        if (CategoryInfo.bVisible == false)
        {
            continue;
        }

        FLogCategoryBase* Category = CategoryInfo.LogCategory;

        ImGui::PushID(Index);
        const char* CategoryFriendlyName = TCHAR_TO_ANSI(*CategoryInfo.GetDisplayName());
        const char* CategoryDescription = TCHAR_TO_ANSI(*CategoryInfo.Description);

        if (bShowAllVerbosity == false)
        {
            const bool IsControlDown = ImGui::GetIO().KeyCtrl;
            if (IsClient)
            {
                ELogVerbosity::Type Verbosity = FCogDebugLog::GetServerVerbosity(CategoryName);
                bool IsActive = FCogDebugLog::IsVerbosityActive(Verbosity);

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255, 0, 0, 200));
                }

                if (ImGui::Checkbox("##Server", &IsActive))
                {
                    ELogVerbosity::Type NewVerbosity;
                    if (IsControlDown && Verbosity != ELogVerbosity::VeryVerbose)
                    {
                        NewVerbosity = ELogVerbosity::VeryVerbose;
                    }
                    else
                    {
                        NewVerbosity = IsActive ? ELogVerbosity::Verbose : ELogVerbosity::Warning;
                    }
                    FCogDebugLog::SetServerVerbosity(*World, CategoryName, NewVerbosity);
                }

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PopStyleColor(1);
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
                {
                    ImGui::BeginTooltip();
                    ImGui::Separator();
                    ImGui::Text("%s", CategoryDescription);
                    ImGui::Text("Server");
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f), "Very Verbose   [CTRL]");
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }

            {
                ELogVerbosity::Type Verbosity = Category->GetVerbosity();
                bool IsActive = FCogDebugLog::IsVerbosityActive(Verbosity);

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255, 128, 0, 200));
                }

                if (ImGui::Checkbox(CategoryFriendlyName, &IsActive))
                {
                    ELogVerbosity::Type NewVerbosity;
                    if (IsControlDown && Verbosity != ELogVerbosity::VeryVerbose)
                    {
                        NewVerbosity = ELogVerbosity::VeryVerbose;
                    }
                    else
                    {
                        NewVerbosity = IsActive ? ELogVerbosity::Verbose : ELogVerbosity::Warning;
                    }
                    Category->SetVerbosity(NewVerbosity);
                }

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PopStyleColor(1);
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", CategoryDescription);
                    ImGui::Separator();
                    if (IsClient)
                    {
                        ImGui::Text("Local Client");
                    }
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f), "Very Verbose   [CTRL]");
                    ImGui::EndTooltip();
                }
            }
        }
        else
        {
            if (IsClient)
            {
                ELogVerbosity::Type CurrentVerbosity = FCogDebugLog::GetServerVerbosity(CategoryName);
                FCogWindowWidgets::SetNextItemToShortWidth();
                if (ImGui::BeginCombo("##Server", FCogDebugHelper::VerbosityToString(CurrentVerbosity)))
                {
                    for (int32 i = (int32)ELogVerbosity::Error; i <= (int32)ELogVerbosity::VeryVerbose; ++i)
                    {
                        bool IsSelected = i == (int32)CurrentVerbosity;
                        ELogVerbosity::Type Verbosity = (ELogVerbosity::Type)i;

                        if (ImGui::Selectable(FCogDebugHelper::VerbosityToString(Verbosity), IsSelected))
                        {
                            FCogDebugLog::SetServerVerbosity(*World, CategoryName, Verbosity);
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", CategoryDescription);
                    ImGui::Separator();
                    ImGui::Text("Server");
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }

            {
                ELogVerbosity::Type CurrentVerbosity = Category->GetVerbosity();
                FCogWindowWidgets::SetNextItemToShortWidth();
                if (ImGui::BeginCombo("##Local", FCogDebugHelper::VerbosityToString(CurrentVerbosity)))
                {
                    for (int32 i = (int32)ELogVerbosity::Error; i <= (int32)ELogVerbosity::VeryVerbose; ++i)
                    {
                        bool IsSelected = i == (int32)CurrentVerbosity;
                        ELogVerbosity::Type Verbosity = (ELogVerbosity::Type)i;

                        if (ImGui::Selectable(FCogDebugHelper::VerbosityToString(Verbosity), IsSelected))
                        {
                            Category->SetVerbosity(Verbosity);
                        }
                    }
                    ImGui::EndCombo();
                }

                if (IsClient && ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", CategoryDescription);
                    ImGui::Separator();
                    ImGui::Text("Local Client");
                    ImGui::EndTooltip();
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s", CategoryFriendlyName);
        }

        ImGui::PopID();
        Index++;
    }
}
