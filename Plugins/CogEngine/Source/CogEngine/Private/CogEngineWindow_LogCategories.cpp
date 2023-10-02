#include "CogEngineWindow_LogCategories.h"

#include "CogDebugHelper.h"
#include "CogWindowWidgets.h"
#include "CogDebugLogCategoryManager.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_LogCategories::PreRender(ImGuiWindowFlags& WindowFlags)
{
    WindowFlags = ImGuiWindowFlags_MenuBar;
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
            ImGui::Checkbox("Show detailed verbosity", &bShowAllVerbosity);
            ImGui::SameLine();
            FCogWindowWidgets::HelpMarker("Show the verbosity level of each log category.");

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Reset"))
        {
            FCogDebugLogCategoryManager::DeactivateAllLogCateories(*World);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Deactivate all the log categories");
        }

        if (ImGui::MenuItem("Flush"))
        {
            FlushPersistentDebugLines(World);
            FlushDebugStrings(GWorld);
            GEngine->ClearOnScreenDebugMessages();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Clear all the debug drawn on screen");
        }

        ImGui::EndMenuBar();
    }

    const bool IsClient = World->GetNetMode() == NM_Client;

    ImGuiStyle& Style = ImGui::GetStyle();

    int Index = 0;
    for (const auto& Entry : FCogDebugLogCategoryManager::GetLogCategories())
    {
        FName CategoryName = Entry.Key;
        const FCogDebugLogCategoryInfo& CategoryInfo = Entry.Value;
        FLogCategoryBase* Category = CategoryInfo.LogCategory;

        ImGui::PushID(Index);
        FString CategoryFriendlyName = Category->GetCategoryName().ToString();

        if (bShowAllVerbosity == false)
        {
            const bool IsControlDown = ImGui::GetIO().KeyCtrl;
            if (IsClient)
            {
                ELogVerbosity::Type Verbosity = FCogDebugLogCategoryManager::GetServerVerbosity(CategoryName);
                bool IsActive = FCogDebugLogCategoryManager::IsVerbosityActive(Verbosity);

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255, 0, 0, 200));
                }

                if (ImGui::Checkbox("##Server", &IsActive))
                {
                    ELogVerbosity::Type NewVerbosity = IsActive ? (IsControlDown ? ELogVerbosity::VeryVerbose : ELogVerbosity::Verbose) : ELogVerbosity::Warning;
                    FCogDebugLogCategoryManager::SetServerVerbosity(CategoryName, NewVerbosity);
                }

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PopStyleColor(1);
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Server");
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f), "Very Verbose   [CTRL]");
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }

            {
                ELogVerbosity::Type Verbosity = Category->GetVerbosity();
                bool IsActive = FCogDebugLogCategoryManager::IsVerbosityActive(Verbosity);

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255, 0, 0, 200));
                }

                if (ImGui::Checkbox(TCHAR_TO_ANSI(*CategoryFriendlyName), &IsActive))
                {
                    ELogVerbosity::Type NewVerbosity = IsActive ? (IsControlDown ? ELogVerbosity::VeryVerbose : ELogVerbosity::Verbose) : ELogVerbosity::Warning;
                    Category->SetVerbosity(NewVerbosity);
                }

                if (Verbosity == ELogVerbosity::VeryVerbose)
                {
                    ImGui::PopStyleColor(1);
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
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
                ELogVerbosity::Type CurrentVerbosity = FCogDebugLogCategoryManager::GetServerVerbosity(CategoryName);
                ImGui::SetNextItemWidth(FCogWindowWidgets::TextBaseWidth * 12);
                if (ImGui::BeginCombo("##Server", FCogDebugHelper::VerbosityToString(CurrentVerbosity)))
                {
                    for (int32 i = (int32)ELogVerbosity::Error; i <= (int32)ELogVerbosity::VeryVerbose; ++i)
                    {
                        bool IsSelected = i == (int32)CurrentVerbosity;
                        ELogVerbosity::Type Verbosity = (ELogVerbosity::Type)i;

                        if (ImGui::Selectable(FCogDebugHelper::VerbosityToString(Verbosity), IsSelected))
                        {
                            FCogDebugLogCategoryManager::SetServerVerbosity(CategoryName, Verbosity);
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Server");
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }

            {
                ELogVerbosity::Type CurrentVerbosity = Category->GetVerbosity();
                ImGui::SetNextItemWidth(FCogWindowWidgets::TextBaseWidth * 12);
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

                if (IsClient && ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Local Client");
                    ImGui::EndTooltip();
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s", TCHAR_TO_ANSI(*CategoryFriendlyName));
        }

        ImGui::PopID();
        Index++;
    }
}
