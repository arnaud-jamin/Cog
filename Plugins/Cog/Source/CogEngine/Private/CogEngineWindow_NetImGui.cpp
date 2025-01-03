#include "CogEngineWindow_NetImGui.h"

#include "CogImguiContext.h"
#include "CogImguiHelper.h"
#include "CogWindowConsoleCommandManager.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Misc/CoreMisc.h"
#include "NetImgui_Api.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::Initialize()
{
    Super::Initialize();
	bHasMenu = true;
	Config = GetConfig<UCogEngineConfig_NetImGui>();

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.ConnectTo"),
		TEXT("Connect to NetImgui server"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		ConnectTo();
	}));

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.ConnectFrom"),
		TEXT("Listen for NetImgui server connection"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		ConnectFrom();
	}));

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.Disconnect"),
		TEXT("Disconnect from NetImgui server"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		Disconnect();
	}));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::Shutdown()
{
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
void FCogEngineWindow_NetImGui::RenderTick(float DeltaTime)
{
	//------------------------------------------------------
	// Before auto connecting, wait for NetImgui startup, 
	// which require imgui context to be initialized
	//------------------------------------------------------
	if (HasAlreadyTriedToConnectOnDedicatedServer == false
		&& Config->AutoConnectOnDedicatedServer
		&& UCogWindowManager::GetIsNetImguiInitialized()
		&& IsRunningDedicatedServer())
		{
			ConnectFrom();
			HasAlreadyTriedToConnectOnDedicatedServer = true;
		}
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

			ImGui::Separator();

			{
				static char Buffer[256] = "";
				ImStrncpy(Buffer, TCHAR_TO_ANSI(*Config->ClientName), IM_ARRAYSIZE(Buffer));
				if (ImGui::InputText("Client Name", Buffer, IM_ARRAYSIZE(Buffer)))
				{
					Config->ClientName = FString(Buffer);
				}
			}
			ImGui::InputInt("Client Port", &Config->ClientPort);

			ImGui::Separator();
			ImGui::Checkbox("Auto Listen On Dedicated Server", &Config->AutoConnectOnDedicatedServer);

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
			ConnectTo();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Attempt a connection to a remote netImgui server at the provided address.");
		}

		if (ImGui::Button("Listen", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			ConnectFrom();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Start listening for a connection request by a remote netImgui server, on the provided Port.");
		}
	}

#endif // #if NETIMGUI_ENABLED
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::ConnectTo()
{
	FCogImGuiContextScope ImGuiContextScope(GetOwner()->GetContext());

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImGui::ConnectTo | Client:%s | Server:%s | ServerPort:%d"),
		*Config->ClientName,
		*Config->ServerName,
		Config->ServerPort);

	const auto clientName = StringCast<ANSICHAR>(*Config->ClientName);
	const auto serverName = StringCast<ANSICHAR>(*Config->ServerName);
	NetImgui::ConnectToApp(clientName.Get(), serverName.Get(), Config->ServerPort, nullptr, nullptr);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::ConnectFrom()
{
	FCogImGuiContextScope ImGuiContextScope(GetOwner()->GetContext());

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImGui::ConnectFrom | Client:%s | ClientPort:%d"),
		*Config->ClientName,
		Config->ClientPort);

	const auto clientName = StringCast<ANSICHAR>(*Config->ClientName);
	NetImgui::ConnectFromApp(clientName.Get(), Config->ClientPort, nullptr, nullptr);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImGui::Disconnect()
{
	FCogImGuiContextScope ImGuiContextScope(GetOwner()->GetContext());

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImGui::Disconnect"));
	NetImgui::Disconnect();
}

//--------------------------------------------------------------------------------------------------------------------------
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
