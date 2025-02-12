#include "CogEngineWindow_NetEmulation.h"

#include "CogEngineWindow_Stats.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetEmulation::RenderHelp()
{
    ImGui::Text("This window is used to configure the network emulation.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetEmulation::Initialize()
{
    Super::Initialize();
    
    Config = GetConfig<UCogEngineWindowConfig_Stats>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetEmulation::RenderContextMenu()
{
    Config->RenderColorConfig();
    Config->RenderPingConfig();
    Config->RenderPacketLossConfig();
    
    ImGui::Separator();
    FCogWindow::RenderContextMenu();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetEmulation::RenderContent()
{
    Super::RenderContent();

    DrawStats();

    ImGui::Separator();

    DrawControls();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetEmulation::DrawStats()
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    if (const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
    {
        const float Ping = PlayerState->GetPingInMilliseconds();
        ImGui::Text("Ping            ");
        ImGui::SameLine();
        ImGui::TextColored(Config->GetPingColor(Ping), "%0.0fms", Ping);
    }

    if (UNetConnection* Connection = PlayerController->GetNetConnection())
    {
        const float OutPacketLost = Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f;
        ImGui::Text("Packet Loss Out ");
        ImGui::SameLine();
        ImGui::TextColored(Config->GetPacketLossColor(OutPacketLost), "%0.0f%%", OutPacketLost);

        const float InPacketLost = Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f;
        ImGui::Text("Packet Loss In  ");
        ImGui::SameLine();
        ImGui::TextColored(Config->GetPacketLossColor(InPacketLost), "%0.0f%%", InPacketLost);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetEmulation::DrawControls()
{
    FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(GetWorld());

    if (WorldContext.ActiveNetDrivers.Num() == 0)
    {
        return;
    }

    static int32 SelectedIndex = 0;

    if (SelectedIndex >= WorldContext.ActiveNetDrivers.Num())
    {
        SelectedIndex = 0;
    }

    FNamedNetDriver* SelectedNetDriver = &WorldContext.ActiveNetDrivers[SelectedIndex];
    if (SelectedNetDriver == nullptr)
    {
        return;
    }

    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::BeginCombo("Driver", TCHAR_TO_ANSI(*SelectedNetDriver->NetDriver->GetName())))
    {
        int i = 0;
        for (FNamedNetDriver& NamedNetDriver : WorldContext.ActiveNetDrivers)
        {
            if (NamedNetDriver.NetDriver != nullptr)
            {
                if (ImGui::Selectable(TCHAR_TO_ANSI(*NamedNetDriver.NetDriver->GetName())))
                {
                    SelectedIndex = i;
                    SelectedNetDriver = &WorldContext.ActiveNetDrivers[i];
                }
            }
            i++;
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (SelectedNetDriver == nullptr)
    {
        return;
    }

#if DO_ENABLE_NET_TEST

    FPacketSimulationSettings Settings = SelectedNetDriver->NetDriver->PacketSimulationSettings;

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::DragInt("Lag Min", &Settings.PktLagMin, 5.0f, 0, INT_MAX, "%d ms"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "If set lag values will randomly fluctuate between Min and Max.");
    }

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::DragInt("Lag Max", &Settings.PktLagMax, 5.0f, 0, INT_MAX, "%d ms"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "If set lag values will randomly fluctuate between Min and Max.");
    }


    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::SliderInt("Packet Loss", &Settings.PktLoss, 0, 100, "%d%%"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "When set, will cause calls to FlushNet to drop packets.\n"
            "Value is treated as %% of packets dropped (i.e. 0 = None, 100 = All).\n"
            "No general pattern / ordering is guaranteed.\n"
            "Clamped between 0 and 100.\n"
            "Works with all other settings.");
    }

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::SliderInt("Packet Order", &Settings.PktOrder, 0, 100, "%d%%"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "When set, will cause calls to FlushNet to change ordering of packets at random.\n"
            "Value is treated as a bool(i.e. 0 = False, anything else = True). \n"
            "This works by randomly selecting packets to be delayed until a subsequent call to FlushNet.\n"
            "Takes precedence over PktDup and PktLag.");
    }

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::SliderInt("Packet Dup", &Settings.PktDup, 0, 100, "%d%%"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "When set, will cause calls to FlushNet to duplicate packets.\n"
            "Value is treated as %% of packets duplicated(i.e. 0 = None, 100 = All).\n"
            "No general pattern / ordering is guaranteed.\n"
            "Clamped between 0 and 100.\n"
            "Cannot be used with PktOrder or PktLag.");
    }

    ImGui::Separator();

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::DragInt("Incoming Lag Min", &Settings.PktIncomingLagMin, 5.0f, 0, INT_MAX, "%d ms"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "The minimum delay in milliseconds to incoming packets before they are processed");
    }

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::DragInt("Incoming Lag Max", &Settings.PktIncomingLagMax, 5.0f, 0, INT_MAX, "%d ms"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "The maximum  delay in milliseconds to add to incoming packets before they are processed");
    }

    //-------------------------------------------------------------------------------------------
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::SliderInt("Incoming Packet Loss", &Settings.PktIncomingLoss, 0, 100, "%d%%"))
    {
        SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "The ratio of incoming packets that will be dropped to simulate packet loss");
    }

#endif //DO_ENABLE_NET_TEST

}