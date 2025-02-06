#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogEngineWindow_NetImgui.generated.h"

class UCogEngineWindowConfig_NetImgui;

UENUM()
enum class ECogNetImguiAutoConnectionMode : uint8
{
    NoAutoConnect,
    AutoConnect,
    AutoListen,
};

class COGENGINE_API FCogEngineWindow_NetImgui : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

    virtual void Shutdown() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaTime) override;

    void ConnectTo() const;

    void ConnectFrom() const;

    void Disconnect() const;

    void RunServer();

    void CloseServer();

private:

    FString GetClientName() const;

    TObjectPtr<UCogEngineWindowConfig_NetImgui> Config = nullptr;

    bool HasStartedAutoConnection = false;

    FProcHandle ServerProcess;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineWindowConfig_NetImgui : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    virtual void Reset() override;

    UPROPERTY(Config)
    FString ClientName = FString("Cog");

    UPROPERTY(Config)
    int32 ClientPort = 8889;

    UPROPERTY(Config)
    ECogNetImguiAutoConnectionMode AutoConnectOnDedicatedServer = ECogNetImguiAutoConnectionMode::AutoConnect;

    UPROPERTY(Config)
    ECogNetImguiAutoConnectionMode AutoConnectOnListenServer = ECogNetImguiAutoConnectionMode::NoAutoConnect;

    UPROPERTY(Config)
    ECogNetImguiAutoConnectionMode AutoConnectOnClient = ECogNetImguiAutoConnectionMode::NoAutoConnect;

    UPROPERTY(Config)
    ECogNetImguiAutoConnectionMode AutoConnectOnStandalone = ECogNetImguiAutoConnectionMode::NoAutoConnect;

    UPROPERTY(Config)
    FString ServerAddress = FString("127.0.0.1");

    UPROPERTY(Config)
    int32 ServerPort = 8888;

    UPROPERTY(Config)
    FString ServerDirectory = FString("C:\\NetImgui\\Server_Exe");

    UPROPERTY(Config)
    FString ServerExecutable = FString("NetImguiServer.exe");

    UPROPERTY(Config)
    FString ServerArguments = FString("");

    UPROPERTY(Config)
    bool AutoRunServer = false;
};