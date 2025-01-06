#include "CogEngineWindow_NetImGui.h"

#include "CogImguiContext.h"
#include "CogImguiHelper.h"
#include "CogWindowConsoleCommandManager.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/World.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Misc/CoreMisc.h"
#include "NetImgui_Api.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::Initialize()
{
    Super::Initialize();
	
	Config = GetConfig<UCogEngineWindowConfig_NetImgui>();

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.Connect"),
		TEXT("Connect to NetImgui server"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		ConnectTo();
	}));

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.Listen"),
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

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.RunServer"),
		TEXT("Run NetImgui server"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		RunServer();
	}));

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.CloseServer"),
		TEXT("Close NetImgui server"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		CloseServer();
	}));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::Shutdown()
{
	if (NetImgui::IsConnected() || NetImgui::IsConnectionPending())
	{
		NetImgui::Disconnect();
	}

	CloseServer();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::ResetConfig()
{
	Super::ResetConfig();
	Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::RenderHelp()
{
    ImGui::Text("This window manage the connection to the NetImgui server."
		"See https://github.com/sammyfreg/netImgui for more info.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::RenderTick(float DeltaTime)
{
	//------------------------------------------------------
	// Before auto connecting, wait for NetImgui startup, 
	// which require imgui context to be initialized
	//------------------------------------------------------
	if (HasAlreadyTriedToConnect == false && FCogImguiContext::GetIsNetImguiInitialized())
	{
		const ENetMode NetMode = GetWorld()->GetNetMode();
		const bool ShouldConnect = (Config->AutoConnectOnDedicatedServer && NetMode == NM_DedicatedServer)
								|| (Config->AutoConnectOnListenServer && NetMode == NM_ListenServer)
								|| (Config->AutoConnectOnClient && NetMode == NM_Client)
								|| (Config->AutoConnectOnStandalone && NetMode == NM_Standalone);

		if (ShouldConnect)
		{
			if (Config->AutoRunServer)
			{
				ConnectTo();
			}
			else
			{
				ConnectFrom();
			}
		}

		if (Config->AutoRunServer)
		{
			RunServer();
		}

		HasAlreadyTriedToConnect = true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::RenderContent()
{
    Super::RenderContent();

#if NETIMGUI_ENABLED

	//----------------------------------------
	// Status
	//----------------------------------------
	if (NetImgui::IsConnected())
	{
		ImGui::TextUnformatted("Status: Connected");
	}
	else if (NetImgui::IsConnectionPending())
	{
		ImGui::TextUnformatted("Status: Waiting Server");
	}
	else
	{
		ImGui::TextUnformatted("Status: Not Connected");
	}

	//----------------------------------------
	// Connection buttons
	//----------------------------------------
	if (NetImgui::IsConnected())
	{
		if (ImGui::Button("Disconnect", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			NetImgui::Disconnect();
		}
	}
	else if (NetImgui::IsConnectionPending())
	{
		if (ImGui::Button("Cancel Connection", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			NetImgui::Disconnect();
		}
	}
	else 
	{
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

	//----------------------------------------
	// Run/Close server button
	//----------------------------------------
	if (FPlatformProcess::IsProcRunning(ServerProcess))
	{
		if (ImGui::Button("Close Server", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			CloseServer();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Close the NetImgui server executable.");
		}
	}
	else 
	{
		if (ImGui::Button("Run Server", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			RunServer();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Run the NetImgui server executable.");
		}
	}

	//----------------------------------------
	// Settings
	//----------------------------------------
	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Address", Config->ServerAddress);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("NetImgui server application address.");
		}

		FCogWindowWidgets::SetNextItemToShortWidth();
		ImGui::InputInt("Server Port", &Config->ServerPort);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Port of the NetImgui Server application to connect to.");
		}

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Client Name", Config->ClientName);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Client name displayed in the server's clients list.");
		}

		FCogWindowWidgets::SetNextItemToShortWidth();
		ImGui::InputInt("Client Port", &Config->ClientPort);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Port this client should wait for connection from server application.");
		}

		ImGui::Checkbox("Auto Connect on Dedicated Server", &Config->AutoConnectOnDedicatedServer);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Automatically connect to the NetImgui server when launching on dedicated server mode.");
		}

		ImGui::Checkbox("Auto Connect on Listen Server", &Config->AutoConnectOnListenServer);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Automatically connect to the NetImgui server when launching on listen server mode.");
		}

		ImGui::Checkbox("Auto Connect on Client", &Config->AutoConnectOnClient);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Automatically connect to the NetImgui server when launching on client mode.");
		}

		ImGui::Checkbox("Auto Connect on Standalone", &Config->AutoConnectOnStandalone);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Automatically connect to the NetImgui server when launching on standlone mode.");
		}

		ImGui::Checkbox("Auto Run Server", &Config->AutoRunServer);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Automatically run the NetImgui server executable at startup.");
		}

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Exe Path", Config->ServerExePath);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Path to NetImgui server executable path. Used to automatically run the NetImgui server executable.");
		}

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Exe Args", Config->ServerExeArgs);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Argument used when launching the NetImgui server executable.");
		}
	}
#endif // #if NETIMGUI_ENABLED
}


//--------------------------------------------------------------------------------------------------------------------------
FString FCogEngineWindow_NetImgui::GetClientName()
{
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:
		return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("Standalone"));

	case NM_DedicatedServer:
		return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("DedicatedServer"));

	case NM_ListenServer:
		return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("ListenServer"));

	case NM_Client:
		return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("Client"));
	}

	return Config->ClientName;
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::ConnectTo()
{
	FCogImGuiContextScope ImGuiContextScope(GetOwner()->GetContext());

	const FString ClientName = GetClientName();

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImgui::ConnectTo | ClientName:%s | ServerAddress:%s | ServerPort:%d"),
		*ClientName,
		*Config->ServerAddress,
		Config->ServerPort);

	const auto clientName = StringCast<ANSICHAR>(*ClientName);
	const auto serverAddress = StringCast<ANSICHAR>(*Config->ServerAddress);
	NetImgui::ConnectToApp(clientName.Get(), serverAddress.Get(), Config->ServerPort, nullptr, nullptr);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::ConnectFrom()
{
	FCogImGuiContextScope ImGuiContextScope(GetOwner()->GetContext());

	const FString ClientName = GetClientName();

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImgui::ConnectFrom | ClientName:%s | ClientPort:%d"),
		*ClientName,
		Config->ClientPort);

	const auto clientName = StringCast<ANSICHAR>(*ClientName);
	NetImgui::ConnectFromApp(clientName.Get(), Config->ClientPort, nullptr, nullptr);

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::Disconnect()
{
	FCogImGuiContextScope ImGuiContextScope(GetOwner()->GetContext());

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImgui::Disconnect"));
	NetImgui::Disconnect();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::RunServer()
{
	if (FPlatformProcess::IsProcRunning(ServerProcess))
	{
		return;
	}

	if (Config->ServerExePath.IsEmpty())
	{
		return;
	}

	ServerProcess = FPlatformProcess::CreateProc(
		*Config->ServerExePath,
		*Config->ServerExeArgs,
		true,
		false,
		false,
		nullptr,
		0,
		nullptr,
		nullptr
	);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_NetImgui::CloseServer()
{
	if (FPlatformProcess::IsProcRunning(ServerProcess) == false)
	{
		return;
	}

	if (Config->ServerExePath.IsEmpty() == false)
	{
		FPlatformProcess::TerminateProc(ServerProcess);
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// Config
//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindowConfig_NetImgui::Reset()
{
	Super::Reset();

#if NETIMGUI_ENABLED

	ClientName = FString("Cog");
	ClientPort = NetImgui::kDefaultClientPort;
	AutoConnectOnDedicatedServer = true;
	AutoConnectOnListenServer = false;
	AutoConnectOnClient = false;
	AutoConnectOnStandalone = false;

	ServerAddress = FString("127.0.0.1");
	ServerPort = NetImgui::kDefaultServerPort;
	ServerExePath = FString("C:\\NetImgui\\Server_Exe\\NetImguiServer.exe");
	ServerExeArgs = FString("");
	AutoRunServer = false;

#endif
}
