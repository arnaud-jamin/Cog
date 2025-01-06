#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogEngineWindow_NetImgui.generated.h"

class UCogEngineWindowConfig_NetImgui;

class COGENGINE_API FCogEngineWindow_NetImgui : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

    virtual void Shutdown() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaTime);

    void ConnectTo();

    void ConnectFrom();

    void Disconnect();

    void TryStartup();

    void RunServer();

    void CloseServer();

private:

    FString GetClientName();

    TObjectPtr<UCogEngineWindowConfig_NetImgui> Config = nullptr;

    bool HasAlreadyTriedToConnect = false;

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
    bool AutoConnectOnDedicatedServer = true;

    UPROPERTY(Config)
    bool AutoConnectOnListenServer = false;

    UPROPERTY(Config)
    bool AutoConnectOnClient = false;

    UPROPERTY(Config)
    bool AutoConnectOnStandalone = false;

    UPROPERTY(Config)
    FString ServerAddress = FString("127.0.0.1");

    UPROPERTY(Config)
    int32 ServerPort = 8888;

    UPROPERTY(Config)
    FString ServerExePath = FString("C:\\NetImgui\\Server_Exe\\NetImguiServer.exe");

    UPROPERTY(Config)
    FString ServerExeArgs = FString("");

    UPROPERTY(Config)
    bool AutoRunServer = false;
};