#include "CogEngineWindow_NetImGui.h"

#include "CogWindowWidgets.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "NetImgui_Api.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::Initialize()
{
    Super::Initialize();
	bHasMenu = true;
	Config = GetConfig<UCogEngineConfig_NetImGui>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::Shutdown()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::ResetConfig()
{
	Super::ResetConfig();
	Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::RenderHelp()
{
    ImGui::Text("");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::RenderContent()
{
    Super::RenderContent();

#if NETIMGUI_ENABLED

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Settings"))
		{
			{
				static char Buffer[256] = "";
				ImStrncpy(Buffer, TCHAR_TO_ANSI(*Config->ServerName), IM_ARRAYSIZE(Buffer));
				if (ImGui::InputText("Server Name", Buffer, IM_ARRAYSIZE(Buffer)))
				{
					Config->ServerName = FString(Buffer);
				}
			}
			ImGui::InputInt("Server Port", &Config->ServerPort);

			{
				static char Buffer[256] = "";
				ImStrncpy(Buffer, TCHAR_TO_ANSI(*Config->ClientName), IM_ARRAYSIZE(Buffer));
				if (ImGui::InputText("Client Name", Buffer, IM_ARRAYSIZE(Buffer)))
				{
					Config->ClientName = FString(Buffer);
				}
			}
			ImGui::InputInt("Client Port", &Config->ClientPort);

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (NetImgui::IsConnected())
	{
		ImGui::TextUnformatted("Status: Connected");
		if (ImGui::Button("Disconnect"))
		{
			NetImgui::Disconnect();
		}
	}
	else if (NetImgui::IsConnectionPending())
	{
		ImGui::TextUnformatted("Status: Waiting Server");
		if (ImGui::Button("Cancel"))
		{
			NetImgui::Disconnect();
		}
	}
	else // No connection
	{
		ImGui::TextUnformatted("Status: Not Connected");

		if (ImGui::Button("Connect", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			const auto clientName = StringCast<ANSICHAR>(*Config->ClientName);
			const auto serverName = StringCast<ANSICHAR>(*Config->ServerName);
			NetImgui::ConnectToApp(clientName.Get(), serverName.Get(), Config->ServerPort, nullptr, nullptr);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Attempt a connection to a remote netImgui server at the provided address.");
		}

		if (ImGui::Button("Listen", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			const auto clientName = StringCast<ANSICHAR>(*Config->ClientName);
			NetImgui::ConnectFromApp(clientName.Get(), Config->ClientPort, nullptr, nullptr);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Start listening for a connection request by a remote netImgui server, on the provided Port.");
		}
	}

#endif // #if NETIMGUI_ENABLED
}

void UCogEngineConfig_NetImGui::Reset()
{
	Super::Reset();

#if NETIMGUI_ENABLED
	ClientName = FString("cog");
	ServerName = FString("localhost");
	ClientPort = NetImgui::kDefaultClientPort;
	ServerPort = NetImgui::kDefaultServerPort;
#endif
}
