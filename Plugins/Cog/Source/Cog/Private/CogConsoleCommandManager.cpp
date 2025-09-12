#include "CogConsoleCommandManager.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

TMap<FString, FCogCommandInfo> FCogConsoleCommandManager::CommandMap;

//--------------------------------------------------------------------------------------------------------------------------
void FCogConsoleCommandManager::RegisterWorldConsoleCommand(const TCHAR* InName, const TCHAR* InHelp, UWorld* InWorld, const FCogWindowConsoleCommandDelegate& InDelegate)
{
    FCogCommandInfo& commandInfo = CommandMap.FindOrAdd(InName);

    if (commandInfo.Receivers.Num() == 0)
    {
        commandInfo.ConsoleObject = IConsoleManager::Get().RegisterConsoleCommand
        (
            InName,
            InHelp,
            FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
                [InName](const TArray<FString>& Args, UWorld* InCommandWorld)
            {
                FCogCommandInfo* commandInfo = CommandMap.Find(InName);
                if (commandInfo == nullptr)
                { return; }

                FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(InCommandWorld);
                if (WorldContext == nullptr)
                { return; }
                
                for (auto& receiver : commandInfo->Receivers)
                {
                    if (receiver.PIEInstance == WorldContext->PIEInstance)
                    {
                        receiver.Delegate.ExecuteIfBound(Args, InCommandWorld);
                        break;
                    }
                }
            }),
            ECVF_Cheat
        );
    }

    if (const FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(InWorld))
    {
        FCogCommandReceiver& receiver = commandInfo.Receivers.AddDefaulted_GetRef();
        receiver.PIEInstance = WorldContext->PIEInstance;
        receiver.Delegate = InDelegate;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogConsoleCommandManager::UnregisterAllWorldConsoleCommands(const UWorld* InWorld)
{
    FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(InWorld);
    if (WorldContext == nullptr)
    { return; }
    
    for (auto& kv : CommandMap)
    {
        FCogCommandInfo& commandInfo = kv.Value;

        for (int32 i = commandInfo.Receivers.Num() - 1; i >= 0; --i)
        {
            if (commandInfo.Receivers[i].PIEInstance == WorldContext->PIEInstance)
            {
                commandInfo.Receivers.RemoveAt(i);
            }
        }

        if (commandInfo.Receivers.Num() == 0 && commandInfo.ConsoleObject != nullptr)
        {
            IConsoleManager::Get().UnregisterConsoleObject(commandInfo.ConsoleObject);
            commandInfo.ConsoleObject = nullptr;
        }
    }
} 