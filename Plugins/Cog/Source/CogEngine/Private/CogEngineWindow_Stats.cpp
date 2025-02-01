#include "CogEngineWindow_Stats.h"

#include "CogWindowWidgets.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

ImVec4 StatRedColor(1.0f, 0.4f, 0.3f, 1.0f);
ImVec4 StatOrangeColor(1.0f, 0.7f, 0.4f, 1.0f);
ImVec4 StatGreenColor(0.5f, 1.0f, 0.6f, 1.0f);

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::Initialize()
{
    Super::Initialize();

    bHasWidget = true;
    bIsWidgetVisible = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderHelp()
{
    ImGui::Text(
        "This window displays engine stats such as FPS, Ping, Packet Loss. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderContent()
{
    Super::RenderContent();

    extern ENGINE_API float GAverageFPS;
    ImGui::Text("FPS             ");
    ImGui::SameLine();
    ImGui::TextColored(GetFpsColor(GAverageFPS), "%0.0f", GAverageFPS);

    if (const APlayerController* PlayerController = GetLocalPlayerController())
    {
        if (const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
        {
            const float Ping = PlayerState->GetPingInMilliseconds();
            ImGui::Text("Ping            ");
            ImGui::SameLine();
            ImGui::TextColored(GetPingColor(Ping), "%0.0fms", Ping);
        }

        if (const UNetConnection* Connection = PlayerController->GetNetConnection())
        {
            const float OutPacketLost = Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f;
            ImGui::Text("Packet Loss Out ");
            ImGui::SameLine();
            ImGui::TextColored(GetPacketLossColor(OutPacketLost), "%0.0f%%", OutPacketLost);

            const float InPacketLost = Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f;
            ImGui::Text("Packet Loss In  ");
            ImGui::SameLine();
            ImGui::TextColored(GetPacketLossColor(InPacketLost), "%0.0f%%", InPacketLost);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidget()
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    const UNetConnection* Connection = PlayerController != nullptr ? PlayerController->GetNetConnection() : nullptr;
    
    RenderMainMenuWidgetFramerate(FCogWindowWidgets::GetFontWidth() * 8);
    RenderMainMenuWidgetPing(Connection != nullptr ? FCogWindowWidgets::GetFontWidth() * 7 : 0.0f); 
    RenderMainMenuWidgetPacketLoss(Connection != nullptr ? FCogWindowWidgets::GetFontWidth() * 7 : 0.0f); 
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidgetFramerate(const float Width)
{
    extern ENGINE_API float GAverageFPS;
    const int32 Fps = (int32)GAverageFPS;

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, GetFpsColor(Fps));
    const bool Open = ImGui::BeginMenu(TCHAR_TO_ANSI(*FString::Printf(TEXT("%3dfps###FramerateButton"), Fps)));
    ImGui::PopStyleColor(2);

    if (Open)
    {
        ImGui::Text("Fps");
        ImGui::SameLine();

        int32 MaxFps = GEngine->GetMaxFPS();
        TArray<int32> Values{ 0, 10, 20, 30, 60, 120 };
        if (FCogWindowWidgets::MultiChoiceButtonsInt(Values, MaxFps, ImVec2(3.5f * FCogWindowWidgets::GetFontWidth(), 0)))
        {
            GEngine->SetMaxFPS(MaxFps);
        }

        ImGui::EndMenu();
    }

    ImGui::SetItemTooltip("Framerate");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidgetPing(const float Width)
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    const APlayerState* PlayerState = PlayerController != nullptr ? PlayerController->GetPlayerState<APlayerState>() : nullptr;
    if (PlayerState == nullptr)
    {
        return;
    }

    const float Ping = PlayerState->GetPingInMilliseconds();
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, GetPingColor(Ping));
    const bool Open = ImGui::BeginMenu(TCHAR_TO_ANSI(*FString::Printf(TEXT("%3dms###PingButton"), (int32)Ping)));
    ImGui::PopStyleColor(2);
    ImGui::SetItemTooltip("Ping");

#if DO_ENABLE_NET_TEST
    if (Open)
    {
        FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(GetWorld());
        if (WorldContext.ActiveNetDrivers.Num() > 0)
        {
            ImGui::Text("Ping");
            ImGui::SameLine();

            const FNamedNetDriver* SelectedNetDriver = &WorldContext.ActiveNetDrivers[0];
            FPacketSimulationSettings Settings = SelectedNetDriver->NetDriver->PacketSimulationSettings;
            TArray<int32> Values{ 0, 50, 100, 200, 500, 1000 };
            if (FCogWindowWidgets::MultiChoiceButtonsInt(Values, Settings.PktIncomingLagMin, ImVec2(4.5f * FCogWindowWidgets::GetFontWidth(), 0)))
            {
                SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
            }
        }

        ImGui::EndMenu();
    }
#endif //DO_ENABLE_NET_TEST

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidgetPacketLoss(const float Width)
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    const UNetConnection* Connection = PlayerController != nullptr ? PlayerController->GetNetConnection() : nullptr;
    if (Connection == nullptr)
    {
        return;
    }

    const float OutPacketLost = Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f;
    const float InPacketLost = Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f;
    const float TotalPacketLost = (OutPacketLost + InPacketLost) / 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, GetPacketLossColor(TotalPacketLost));

    if (ImGui::Button(TCHAR_TO_ANSI(*FString::Printf(TEXT("%2d%%###PacketLossButton"), (int32)TotalPacketLost)), ImVec2(Width, 0.0f)))
    {
        ImGui::OpenPopup("PacketLossPopup");
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);

    ImGui::SetItemTooltip("Packet Loss");

#if DO_ENABLE_NET_TEST
    if (ImGui::BeginPopup("PacketLossPopup"))
    {
        FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(GetWorld());
        if (WorldContext.ActiveNetDrivers.Num() > 0)
        {
            ImGui::Text("Packet Loss");
            ImGui::SameLine();

            const FNamedNetDriver* SelectedNetDriver = &WorldContext.ActiveNetDrivers[0];
            FPacketSimulationSettings Settings = SelectedNetDriver->NetDriver->PacketSimulationSettings;

            TArray<int32> Values{ 0, 5, 10, 20, 30, 40, 50 };
            if (FCogWindowWidgets::MultiChoiceButtonsInt(Values, Settings.PktIncomingLoss, ImVec2(3.5f * FCogWindowWidgets::GetFontWidth(), 0)))
            {
                Settings.PktLoss = Settings.PktIncomingLoss;
                SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
            }
        }
        ImGui::EndPopup();
    }
#endif //DO_ENABLE_NET_TEST
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogEngineWindow_Stats::GetFpsColor(const float Value, const float Good /*= 50.0f*/, const float Medium /*= 30.0f*/)
{
    if (Value > Good)
    {
        return StatGreenColor;
    }

    if (Value > Medium)
    {
        return StatOrangeColor;
    }

    return StatRedColor;
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogEngineWindow_Stats::GetPingColor(const float Value, const float Good /*= 100.0f*/, const float Medium /*= 200.0f*/)
{
    if (Value > Medium)
    {
        return StatRedColor;
    }

    if (Value > Good)
    {
        return StatOrangeColor;
    }

    return StatGreenColor;
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogEngineWindow_Stats::GetPacketLossColor(const float Value, const float Good /*= 10.0f*/, const float Medium /*= 20.0f*/)
{
    if (Value > Medium)
    {
        return StatRedColor;
    }

    if (Value > Good)
    {
        return StatOrangeColor;
    }

    return StatGreenColor;
}