#include "CogEngineWindow_Stats.h"

#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "GameFramework/PlayerState.h"

ImVec4 StatRedColor(1.0f, 0.4f, 0.3f, 1.0f);
ImVec4 StatOrangeColor(1.0f, 0.7f, 0.4f, 1.0f);
ImVec4 StatGreenColor(0.5f, 1.0f, 0.6f, 1.0f);

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Stats::RenderContent()
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

        if (UNetConnection* Connection = PlayerController->GetNetConnection())
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
void UCogEngineWindow_Stats::DrawMainMenuWidget(bool Draw, float& Width)
{
    const float ResetButtonWidth = FCogWindowWidgets::TextBaseWidth * 5;
    Width = FCogWindowWidgets::TextBaseWidth * 15;

    if (Draw == false)
    {
        return;
    }

    extern ENGINE_API float GAverageFPS;
    ImGui::TextColored(GetFpsColor(GAverageFPS), "%3dfps ", (int32)GAverageFPS);

    if (const APlayerController* PlayerController = GetLocalPlayerController())
    {
        if (const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
        {
            const float Ping = PlayerState->GetPingInMilliseconds();
            ImGui::SameLine();
            ImGui::TextColored(GetPingColor(Ping), "%3dms ", (int32)Ping);
        }

        if (UNetConnection* Connection = PlayerController->GetNetConnection())
        {
            const float OutPacketLost = Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f;
            const float InPacketLost = Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f;
            const float TotalPacketLost = OutPacketLost + InPacketLost;
            ImGui::SameLine();
            ImGui::TextColored(GetPacketLossColor(TotalPacketLost), "%2d%% ", (int32)TotalPacketLost);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 UCogEngineWindow_Stats::GetFpsColor(float Value, float Good /*= 50.0f*/, float Medium /*= 30.0f*/)
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
ImVec4 UCogEngineWindow_Stats::GetPingColor(float Value, float Good /*= 100.0f*/, float Medium /*= 200.0f*/)
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
ImVec4 UCogEngineWindow_Stats::GetPacketLossColor(float Value, float Good /*= 10.0f*/, float Medium /*= 20.0f*/)
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