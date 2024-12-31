#include "CogWindowConsoleCommandManager.h"

#include "Engine/World.h"

TMap<FString, FCogCommandInfo> FCogWindowConsoleCommandManager::CommandMap;

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowConsoleCommandManager::RegisterWorldConsoleCommand(const TCHAR* InName, const TCHAR* InHelp, UWorld* InWorld, const FCogWindowConsoleCommandDelegate& InDelegate)
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
                {
                    return;
                }

                for (auto& receiver : commandInfo->Receivers)
                {
                    if (receiver.World == InCommandWorld)
                    {
                        receiver.Delegate.ExecuteIfBound(Args, InCommandWorld);
                        break;
                    }
                }
            }),
            ECVF_Cheat
        );
    }

    FCogCommandReceiver& receiver = commandInfo.Receivers.AddDefaulted_GetRef();
    receiver.World = InWorld;
    receiver.Delegate = InDelegate;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowConsoleCommandManager::UnregisterAllWorldConsoleCommands(const UWorld* InWorld)
{
    for (auto& kv : CommandMap)
    {
        FCogCommandInfo& commandInfo = kv.Value;

        for (int32 i = commandInfo.Receivers.Num() - 1; i >= 0; --i)
        {
            if (commandInfo.Receivers[i].World == InWorld)
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