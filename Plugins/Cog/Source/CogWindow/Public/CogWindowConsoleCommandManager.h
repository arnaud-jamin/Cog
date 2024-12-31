#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "Templates/Function.h"

class UWorld;

DECLARE_DELEGATE_TwoParams(FCogWindowConsoleCommandDelegate, const TArray<FString>&, UWorld*);

//--------------------------------------------------------------------------------------------------------------------------
struct FCogCommandReceiver
{
    UWorld* World = nullptr;

    FCogWindowConsoleCommandDelegate Delegate;
};

//--------------------------------------------------------------------------------------------------------------------------
struct FCogCommandInfo
{
    IConsoleObject* ConsoleObject = nullptr;

    TArray<FCogCommandReceiver> Receivers;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGWINDOW_API FCogWindowConsoleCommandManager
{
public:

    static void RegisterWorldConsoleCommand(const TCHAR* InName, const TCHAR* InHelp, UWorld* InWorld, const FCogWindowConsoleCommandDelegate& InDelegate);
    
    static void UnregisterAllWorldConsoleCommands(const UWorld* InWorld);

protected:

    static TMap<FString, FCogCommandInfo> CommandMap;
};
