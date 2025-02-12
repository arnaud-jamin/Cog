#include "CogEngineWindow_LogCategories.h"

#include "CogDebugHelper.h"
#include "CogDebug.h"
#include "CogWidgets.h"
#include "CogDebugLog.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_LogCategories::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_LogCategories::RenderHelp()
{
    ImGui::Text(
        "This window can be used to activate and deactivate log categories. "
        "Activating a log category set its verbosity to Verbose, or VeryVerbose when CTRL is pressed. "
        "Deactivating a log category set its verbosity to Warning. "
        "The detailed verbosity of each log category can shown by using the Option menu. "
        "On a client, both the client and the server verbosity can be modified. "
        "The log categories are used to display both output log and debug display in the world. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_LogCategories::ResetConfig()
{
    Super::ResetConfig();

    FCogDebugLog::DeactivateAllLogCategories(*GetWorld());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_LogCategories::RenderContent()
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
                FCogDebugLog::DeactivateAllLogCategories(*World);
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

        bool bIsFilteringBySelection = FCogDebug::GetIsFilteringBySelection();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
        FCogWidgets::PushStyleCompact();
        if (ImGui::Checkbox("Filter", &bIsFilteringBySelection))
        {
            FCogDebug::SetIsFilteringBySelection(GetWorld(), bIsFilteringBySelection);
        }
        FCogWidgets::PopStyleCompact();

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

    if (FCogDebugLog::GetLogCategories().Num() == 0)
    {
        ImGui::Text("No log categories have been added.");
        return;
    }

    const bool IsClient = World->GetNetMode() == NM_Client;

    int Index = 0;
    for (const auto& Entry : FCogDebugLog::GetLogCategories())
    {
	    const FName CategoryName = Entry.Key;
        const FCogDebugLogCategoryInfo& CategoryInfo = Entry.Value;
        if (CategoryInfo.bVisible == false)
        {
            continue;
        }

        FLogCategoryBase* Category = CategoryInfo.LogCategory;

        ImGui::PushID(Index);
        const auto CategoryFriendlyName = StringCast<ANSICHAR>(*CategoryInfo.GetDisplayName());
        const auto CategoryDescription = StringCast<ANSICHAR>(*CategoryInfo.Description);

        if (bShowAllVerbosity == false)
        {
            const bool IsControlDown = ImGui::GetIO().KeyCtrl;
            if (IsClient)
            {
	            const ELogVerbosity::Type Verbosity = FCogDebugLog::GetServerVerbosity(CategoryName);
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
                    ImGui::Text("%s", CategoryDescription.Get());
                    ImGui::Text("Server");
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f), "Very Verbose   [CTRL]");
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }

            {
	            const ELogVerbosity::Type Verbosity = Category->GetVerbosity();
                bool IsActive = FCogDebugLog::IsVerbosityActive(Verbosity);

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255, 128, 0, 200));
                }

                if (ImGui::Checkbox(CategoryFriendlyName.Get(), &IsActive))
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
                    ImGui::Text("%s", CategoryDescription.Get());
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
	            const ELogVerbosity::Type CurrentVerbosity = FCogDebugLog::GetServerVerbosity(CategoryName);
                FCogWidgets::SetNextItemToShortWidth();
                if (ImGui::BeginCombo("##Server", FCogDebugHelper::VerbosityToString(CurrentVerbosity)))
                {
                    for (int32 i = ELogVerbosity::Error; i <= static_cast<int32>(ELogVerbosity::VeryVerbose); ++i)
                    {
	                    const bool IsSelected = i == static_cast<int32>(CurrentVerbosity);
	                    const ELogVerbosity::Type Verbosity = static_cast<ELogVerbosity::Type>(i);

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
                    ImGui::Text("%s", CategoryDescription.Get());
                    ImGui::Separator();
                    ImGui::Text("Server");
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }

            {
	            const ELogVerbosity::Type CurrentVerbosity = Category->GetVerbosity();
                FCogWidgets::SetNextItemToShortWidth();
                if (ImGui::BeginCombo("##Local", FCogDebugHelper::VerbosityToString(CurrentVerbosity)))
                {
                    for (int32 i = ELogVerbosity::Error; i <= static_cast<int32>(ELogVerbosity::VeryVerbose); ++i)
                    {
	                    const bool IsSelected = i == static_cast<int32>(CurrentVerbosity);
	                    const ELogVerbosity::Type Verbosity = static_cast<ELogVerbosity::Type>(i);

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
                    ImGui::Text("%s", CategoryDescription.Get());
                    ImGui::Separator();
                    ImGui::Text("Local Client");
                    ImGui::EndTooltip();
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s", CategoryFriendlyName.Get());
        }

        ImGui::PopID();
        Index++;
    }
}
