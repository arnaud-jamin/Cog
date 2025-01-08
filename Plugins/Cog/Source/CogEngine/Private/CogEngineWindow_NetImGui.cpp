#include "CogEngineWindow_NetImGui.h"

#include "CogImguiContext.h"
#include "CogImguiHelper.h"
#include "CogWindowConsoleCommandManager.h"
#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/World.h"
#include "imgui.h"
#include "Misc/CoreMisc.h"
#include "Misc/Paths.h"
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
		TEXT("Run NetImgui server application"),
		GetWorld(),
		FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
	{
		RunServer();
	}));

	FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(
		TEXT("Cog.NetImgui.CloseServer"),
		TEXT("Close NetImgui server application"),
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

	if (HasStartedAutoConnection == false && FCogImguiContext::GetIsNetImguiInitialized())
	{
		HasStartedAutoConnection = true;

		if (Config->AutoRunServer && GetWorld()->IsPlayInEditor())
		{
			RunServer();
		}

		ECogNetImguiAutoConnectionMode AutoConnectMode = ECogNetImguiAutoConnectionMode::NoAutoConnect;
		switch (GetWorld()->GetNetMode())
		{
			case NM_Client:				AutoConnectMode = Config->AutoConnectOnClient; break;
			case NM_ListenServer:		AutoConnectMode = Config->AutoConnectOnListenServer; break;
			case NM_Standalone:			AutoConnectMode = Config->AutoConnectOnStandalone; break;
			case NM_DedicatedServer:	AutoConnectMode = Config->AutoConnectOnDedicatedServer; break;
			default:					AutoConnectMode = ECogNetImguiAutoConnectionMode::NoAutoConnect;
		}

		if (AutoConnectMode == ECogNetImguiAutoConnectionMode::AutoListen)
		{
			ConnectFrom();
		}
		else if (AutoConnectMode == ECogNetImguiAutoConnectionMode::AutoConnect)
		{
			ConnectTo();
		}
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
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.6f, 1.0f), "Connected");
	}
	else if (NetImgui::IsConnectionPending())
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Waiting Server");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.3f, 1.0f), "Not Connected");
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
		ImGui::SetItemTooltip("Attempt a connection to a remote netImgui server at the provided address.");

		if (ImGui::Button("Listen", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			ConnectFrom();
		}
		ImGui::SetItemTooltip("Start listening for a connection request by a remote netImgui server, on the provided Port.");
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
		if (NetImgui::IsConnected() == false)
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
	}

	ImGui::Spacing();

	//----------------------------------------
	// Settings
	//----------------------------------------
	if (ImGui::CollapsingHeader("Settings"))
	{
		ImGui::SeparatorText("Connection");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Address", Config->ServerAddress);
		ImGui::SetItemTooltip("NetImgui server application address.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		ImGui::InputInt("Server Port", &Config->ServerPort);
		ImGui::SetItemTooltip("Port of the NetImgui Server application to connect to.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Client Name", Config->ClientName);
		ImGui::SetItemTooltip("Client name displayed in the server's clients list.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		ImGui::InputInt("Client Port", &Config->ClientPort);
		ImGui::SetItemTooltip("Port this client should wait for connection from server application.");

		ImGui::SeparatorText("Auto-Connect");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::ComboboxEnum("Dedicated Server", Config->AutoConnectOnDedicatedServer);
		ImGui::SetItemTooltip("Auto-connect mode to the NetImgui server when launching on dedicated server mode.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::ComboboxEnum("Listen Server", Config->AutoConnectOnListenServer);
		ImGui::SetItemTooltip("Auto-connect mode to the NetImgui server when launching on listen server mode.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::ComboboxEnum("Client", Config->AutoConnectOnClient);
		ImGui::SetItemTooltip("Auto-connect mode to the NetImgui server when launching on client mode.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::ComboboxEnum("Standalone", Config->AutoConnectOnStandalone);
		ImGui::SetItemTooltip("Auto-connect mode to the NetImgui server when launching on standalone mode.");

		ImGui::SeparatorText("Server App");

		ImGui::Checkbox("Auto Run Server", &Config->AutoRunServer);
		ImGui::SetItemTooltip("Automatically run the NetImgui server executable at startup.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Executable", Config->ServerExecutable);
		ImGui::SetItemTooltip("Filename of the NetImgui server executable.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Directory", Config->ServerDirectory);
		ImGui::SetItemTooltip("Directory of the NetImgui server executable.");

		FCogWindowWidgets::SetNextItemToShortWidth();
		FCogWindowWidgets::InputText("Server Arguments", Config->ServerArguments);
		ImGui::SetItemTooltip("Argument used when launching the NetImgui server executable.");
	}
#endif // #if NETIMGUI_ENABLED
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogEngineWindow_NetImgui::GetClientName() const
{
	switch (GetWorld()->GetNetMode())
	{
		case NM_Standalone:			return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("Standalone"));
		case NM_DedicatedServer:	return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("DedicatedServer"));
		case NM_ListenServer:		return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("ListenServer"));
		case NM_Client:				return FString::Printf(TEXT("%s_%s"), *Config->ClientName, TEXT("Client"));
		default:;
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
		UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImgui::RunServer | Already running"));
		return;
	}

	const FString ServerPath = FPaths::Combine(Config->ServerDirectory, Config->ServerExecutable);
	if (IPlatformFile::GetPlatformPhysical().FileExists(*ServerPath) == false)
	{
		UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImgui::RunServer | Invalid Server Path:%s"), *ServerPath);
		return;
	}

	UE_LOG(LogCogImGui, Verbose, TEXT("FCogEngineWindow_NetImgui::RunServer | Server Path:%s"), *ServerPath);

	ServerProcess = FPlatformProcess::CreateProc(
		*ServerPath,
		*Config->ServerArguments,
		false,
		false,
		false,
		nullptr,
		0,
		*Config->ServerDirectory,
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

	FPlatformProcess::TerminateProc(ServerProcess);
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
	AutoConnectOnDedicatedServer = ECogNetImguiAutoConnectionMode::AutoConnect;
	AutoConnectOnListenServer = ECogNetImguiAutoConnectionMode::NoAutoConnect;
	AutoConnectOnClient = ECogNetImguiAutoConnectionMode::NoAutoConnect;
	AutoConnectOnStandalone = ECogNetImguiAutoConnectionMode::NoAutoConnect;

	ServerAddress = FString("127.0.0.1");
	ServerPort = NetImgui::kDefaultServerPort;
	ServerDirectory = FString("C:\\NetImgui\\Server_Exe");
	ServerExecutable = FString("NetImguiServer.exe");
	ServerArguments = FString("");
	AutoRunServer = false;

#endif
}
