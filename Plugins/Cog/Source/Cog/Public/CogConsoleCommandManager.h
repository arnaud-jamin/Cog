#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

class UWorld;

DECLARE_DELEGATE_TwoParams(FCogWindowConsoleCommandDelegate, const TArray<FString>&, UWorld*);

//--------------------------------------------------------------------------------------------------------------------------
struct FCogCommandReceiver
{
    int32 PIEInstance = INDEX_NONE;

    FCogWindowConsoleCommandDelegate Delegate;
};

//--------------------------------------------------------------------------------------------------------------------------
struct FCogCommandInfo
{
    IConsoleObject* ConsoleObject = nullptr;

    TArray<FCogCommandReceiver> Receivers;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COG_API FCogConsoleCommandManager
{
    static void RegisterWorldConsoleCommand(const TCHAR* InName, const TCHAR* InHelp, UWorld* InWorld, const FCogWindowConsoleCommandDelegate& InDelegate);
    
    static void UnregisterAllWorldConsoleCommands(const UWorld* InWorld);

protected:

    static TMap<FString, FCogCommandInfo> CommandMap;
};
