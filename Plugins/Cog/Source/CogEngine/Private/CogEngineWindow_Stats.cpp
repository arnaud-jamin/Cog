#include "CogEngineWindow_Stats.h"

#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"



//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::Initialize()
{
    Super::Initialize();

    bHasWidget = true;
    bIsWidgetVisible = true;

    Config = GetConfig<UCogEngineWindowConfig_Stats>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderHelp()
{
    ImGui::Text(
        "This window displays engine stats such as FPS, Ping, Packet Loss. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderContextMenu()
{
    Config->RenderAllConfigs();
    
    ImGui::Separator();
    FCogWindow::RenderContextMenu();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderContent()
{
    Super::RenderContent();

    extern ENGINE_API float GAverageFPS;
    ImGui::Text("FPS             ");
    ImGui::SameLine();
    ImGui::TextColored(Config->GetFpsColor(GAverageFPS), "%0.0f", GAverageFPS);

    if (const APlayerController* PlayerController = GetLocalPlayerController())
    {
        if (const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
        {
            const float Ping = PlayerState->GetPingInMilliseconds();
            ImGui::Text("Ping            ");
            ImGui::SameLine();
            ImGui::TextColored(Config->GetPingColor(Ping), "%0.0fms", Ping);
        }

        if (const UNetConnection* Connection = PlayerController->GetNetConnection())
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
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidget()
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    
    RenderMainMenuWidgetFrameRate();

    const UNetConnection* Connection = PlayerController != nullptr ? PlayerController->GetNetConnection() : nullptr;
    if (Connection != nullptr)
    {
        RenderMainMenuWidgetPing();
        
        RenderMainMenuWidgetPacketLoss();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidgetFrameRate()
{
    extern ENGINE_API float GAverageFPS;
    const int32 Fps = static_cast<int32>(GAverageFPS);

    ImGui::PushStyleColor(ImGuiCol_Text, Config->GetFpsColor(Fps));
    const bool Open = ImGui::BeginMenu(TCHAR_TO_ANSI(*FString::Printf(TEXT("%3dfps###FrameRateButton"), Fps)));
    const float Width = ImGui::GetItemRectSize().x;
    ImGui::PopStyleColor(1);

    if (ImGui::BeginPopupContextItem())
    {
        Config->RenderColorConfig();
        Config->RenderFrameRateConfig();
        Super::RenderContextMenu();
        ImGui::EndPopup();
    }

    if (Open == false)
    {
        ImGui::SetItemTooltip("Frame Rate");
    }
    
    if (Open)
    {
        const int32 MaxFps = GEngine->GetMaxFPS();
        for (int32 i = 0; i < Config->FrameRates.Num(); ++i)
        {
            ImGui::PushID(i);
            const float Value = Config->FrameRates[i];
            const auto ValueText = StringCast<ANSICHAR>(*FCogWidgets::FormatSmallFloat(Value));
            if (ImGui::Selectable(ValueText.Get(), Value == MaxFps, ImGuiSelectableFlags_None, ImVec2(Width, 0)))
            {
                GEngine->SetMaxFPS(Value);
            }
            ImGui::PopID();
        }
        
        ImGui::EndMenu();
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidgetPing()
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    const APlayerState* PlayerState = PlayerController != nullptr ? PlayerController->GetPlayerState<APlayerState>() : nullptr;
    if (PlayerState == nullptr)
    { return; }

    const int32 Ping = static_cast<int32>(PlayerState->GetPingInMilliseconds());
    ImGui::PushStyleColor(ImGuiCol_Text, Config->GetPingColor(Ping));
    const bool Open = ImGui::BeginMenu(TCHAR_TO_ANSI(*FString::Printf(TEXT("%3dms###PingButton"), Ping)));
    const float Width = ImGui::GetItemRectSize().x;
    ImGui::PopStyleColor(1);
    
    if (ImGui::BeginPopupContextItem())
    {
        Config->RenderColorConfig();
        Config->RenderPingConfig();
        Super::RenderContextMenu();
        ImGui::EndPopup();
    }

    if (Open == false)
    {
        ImGui::SetItemTooltip("Ping");
    }
    
#if DO_ENABLE_NET_TEST
    if (Open)
    {
        FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(GetWorld());
        if (WorldContext.ActiveNetDrivers.Num() > 0)
        {
            const FNamedNetDriver* SelectedNetDriver = &WorldContext.ActiveNetDrivers[0];
            FPacketSimulationSettings Settings = SelectedNetDriver->NetDriver->PacketSimulationSettings;
            
            for (int32 i = 0; i < Config->Pings.Num(); ++i)
            {
                ImGui::PushID(i);
                const float Value = Config->Pings[i];
                const auto ValueText = StringCast<ANSICHAR>(*FCogWidgets::FormatSmallFloat(Value));
                if (ImGui::Selectable(ValueText.Get(), Value == Settings.PktIncomingLagMin, ImGuiSelectableFlags_None, ImVec2(Width, 0)))
                {
                    Settings.PktIncomingLagMin = Value;
                    SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
                }
                
                ImGui::PopID();
            }
        }
        ImGui::EndMenu();
    }
#endif //DO_ENABLE_NET_TEST
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Stats::RenderMainMenuWidgetPacketLoss()
{
    const APlayerController* PlayerController = GetLocalPlayerController();
    const UNetConnection* Connection = PlayerController != nullptr ? PlayerController->GetNetConnection() : nullptr;
    if (Connection == nullptr)
    { return; }

    const float OutPacketLost = Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f;
    const float InPacketLost = Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f;
    const float TotalPacketLost = (OutPacketLost + InPacketLost) / 2;

    ImGui::PushStyleColor(ImGuiCol_Text, Config->GetPacketLossColor(TotalPacketLost));
    const bool Open = ImGui::BeginMenu(TCHAR_TO_ANSI(*FString::Printf(TEXT("%2d%% ###PacketLossButton"), static_cast<int32>(TotalPacketLost))));
    const float Width = ImGui::GetItemRectSize().x;
    ImGui::PopStyleColor(1);

    if (ImGui::BeginPopupContextItem())
    {
        Config->RenderColorConfig();
        Config->RenderPacketLossConfig();
        Super::RenderContextMenu();
        ImGui::EndPopup();
    }
    
    if (Open == false)
    {
        ImGui::SetItemTooltip("Packet Loss");
    }
    
#if DO_ENABLE_NET_TEST
    if (Open)
    {
        FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(GetWorld());
        if (WorldContext.ActiveNetDrivers.Num() > 0)
        {
            const FNamedNetDriver* SelectedNetDriver = &WorldContext.ActiveNetDrivers[0];
            FPacketSimulationSettings Settings = SelectedNetDriver->NetDriver->PacketSimulationSettings;

            for (int32 i = 0; i < Config->PacketLosses.Num(); ++i)
            {
                ImGui::PushID(i);
                const float Value = Config->PacketLosses[i];
                const auto ValueText = StringCast<ANSICHAR>(*FCogWidgets::FormatSmallFloat(Value));
                if (ImGui::Selectable(ValueText.Get(), Value == Settings.PktIncomingLagMin, ImGuiSelectableFlags_None, ImVec2(Width, 0)))
                {
                    Settings.PktIncomingLoss = Value;
                    Settings.PktLoss = Settings.PktIncomingLoss;
                    SelectedNetDriver->NetDriver->SetPacketSimulationSettings(Settings);
                }
                
                ImGui::PopID();
            }
        }
        ImGui::EndPopup();
    }
#endif //DO_ENABLE_NET_TEST
}


//--------------------------------------------------------------------------------------------------------------------------
ImVec4 UCogEngineWindowConfig_Stats::GetFpsColor(const float Value) const
{
    if (Value > GoodFrameRate)
    { return FCogImguiHelper::ToImVec4(GoodColor); }

    if (Value > MediumFrameRate)
    { return FCogImguiHelper::ToImVec4(MediumColor); }

    return FCogImguiHelper::ToImVec4(BadColor);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 UCogEngineWindowConfig_Stats::GetPingColor(const float Value) const
{
    if (Value > MediumPing)
    { return FCogImguiHelper::ToImVec4(BadColor); }

    if (Value > GoodPing)
    { return FCogImguiHelper::ToImVec4(MediumColor); }

    return FCogImguiHelper::ToImVec4(GoodColor);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 UCogEngineWindowConfig_Stats::GetPacketLossColor(const float Value) const
{
    if (Value > MediumPacketLoss)
    { return FCogImguiHelper::ToImVec4(BadColor); }

    if (Value > GoodPacketLoss)
    { return FCogImguiHelper::ToImVec4(MediumColor); }

    return FCogImguiHelper::ToImVec4(GoodColor);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindowConfig_Stats::RenderAllConfigs()
{
    RenderColorConfig();
    RenderFrameRateConfig();
    RenderPingConfig();
    RenderPacketLossConfig();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindowConfig_Stats::RenderColorConfig()
{
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen))
    {
        constexpr ImGuiColorEditFlags ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
        FCogImguiHelper::ColorEdit4("Good Color", GoodColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of a stat with a good value.");

        FCogImguiHelper::ColorEdit4("Medium Color", MediumColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of a stat with a medium value.");

        FCogImguiHelper::ColorEdit4("Bad Color", BadColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of a stat with a bad value.");
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindowConfig_Stats::RenderFrameRateConfig()
{
    if (ImGui::CollapsingHeader("Frame Rate", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("Good Frame Rate", &GoodFrameRate);
        ImGui::InputInt("Medium Frame Rate", &MediumFrameRate);
        FCogWidgets::IntArray("Max Frame Rate", FrameRates, 10, ImVec2(0, ImGui::GetFontSize() * 10));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindowConfig_Stats::RenderPingConfig()
{
    if (ImGui::CollapsingHeader("Ping", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("Good Ping", &GoodPing);
        ImGui::InputInt("Medium Ping", &MediumPing);
        FCogWidgets::IntArray("Ping Emulation", Pings, 10, ImVec2(0, ImGui::GetFontSize() * 10));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindowConfig_Stats::RenderPacketLossConfig()
{
    if (ImGui::CollapsingHeader("Packet Loss", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("Good Packet Loss", &GoodPacketLoss);
        ImGui::InputInt("Medium Packet Loss", &MediumPacketLoss);
        FCogWidgets::IntArray("Packet Loss Emulation", PacketLosses, 10, ImVec2(0, ImGui::GetFontSize() * 10));
    }
}
