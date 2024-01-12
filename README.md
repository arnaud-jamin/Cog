# Cog
Cog is a set of debug tools for Unreal Engine built on top of [Dear ImGui](https://github.com/ocornut/imgui)

![Cog](https://github.com/arnaud-jamin/Cog/assets/13844285/21659aea-2cd8-4ef6-b3b0-5795f5f3246b)

- [Video](https://www.youtube.com/watch?v=ea5hz3cFcMM)
- [Sample Executable (Windows)](https://drive.google.com/file/d/1T7jQFoZ5rd6goBtDH-FCbjn6Kr1RzUCE/view?usp=sharing) (300 MB)

Cog provides:
- ImGui windows to inspect and configure various Unreal systems (Core Engine, Enhanced Inputs, Gameplay Abilities, AI)
- Window mangement with persistent configuration and layouts.
- C++ and Blueprint functions to log and debug draw within Log Categories.
- Control over the server regarding debug draw, logging, spawning, cheats.

General Info:
- Cog can be used both in editor and package builds. It is disabled by default on shipping builds.
- Press the `[F1]` key or use the `Cog.ToggleInput` console command to open the ImGui Main Menu.
- Many windows display their contents based on a selected actor. The selector actor can be chosen using the `Engine/Selection` window or widget.
- Widgets such as Stats (FPS, Ping), or Actor Selection, can be added in the main menu bar from the 'Window/Widgets" menu.

## Cog Windows

### Abilities 

Displays the gameplay abilities of the selected actor.

[![Abilities](https://github.com/arnaud-jamin/Cog/assets/13844285/cc6cb2af-eb9a-42fd-8ae5-80b5c7b361e9)]()
- Click the ability checkbox to force its activation or deactivation.
- Right click an ability to remove it, or open/close the ability separate window.
- Use the 'Give Ability' menu to manually give an ability from a list defined in a Data Asset.

### Actions
Displays the state of Input Action.

![Actions](https://github.com/arnaud-jamin/Cog/assets/13844285/6323e78b-2ee4-43e2-bec6-19aa15716d2c)
- Can be used to inject inputs to help debugging, as loosing window focus when breaking in the code doesn't affect the state of injected inputs unlike real inputs.
- The display input action are read from a Input Mapping Context defined in a Data Asset.

### Attributes
Displays the gameplay attributes of the selected actor.

![Attributes](https://github.com/arnaud-jamin/Cog/assets/13844285/a6329ef1-f775-4e6e-9581-6389f9f4b39c)
- Attributes can be sorted by name, category or attribute set.
- Attributes with the Current value greater than the Base value are displayed in green.
- Attributes with the Current value lower than the Base value are displayed in red.
- Use the options 'Show Only Modified' to only show the attributes that have modifiers.

### Behavior Tree
Displays the behavior tree of the selected actor.

![Behavior Tree](https://github.com/arnaud-jamin/Cog/assets/13844285/c799e85f-b641-4d6f-9476-54a5cbd73c65)

### Blackboard 
Displays the blackboard of the selected actor.

![Blackboard](https://github.com/arnaud-jamin/Cog/assets/13844285/649d46d5-386c-4990-9f45-e4eb95a6b81a)

### Cheats
Used to apply cheats to the selected actor.

![Cheats](https://github.com/arnaud-jamin/Cog/assets/13844285/b7b89635-7924-49b7-98c0-311199947dfc)
- The cheats are gameplay effects configured in a data asset.
- Key modifiers can be used to change which actor should be affected by the cheat:
  - `[CTRL]` Apply the cheat to the controlled actor
  - `[ALT]` Apply the cheat to the allies of the selected actor
  - `[SHIFT]` Apply the cheat to the enemies of the selected actor

### Collisions Tester
Used to test a collision query

![Collisions Tester](https://github.com/arnaud-jamin/Cog/assets/13844285/12f7cb7e-13c9-4eed-8ea4-f3eb7475cf34)
![Collisions Tester](https://github.com/arnaud-jamin/Cog/assets/13844285/4d01fd01-d0b3-41a7-9344-662f190ebaf0)

### Collisions Viewer
Used to inspect collisions in the world 

![Collisions Viewer](https://github.com/arnaud-jamin/Cog/assets/13844285/ccee855b-b33b-4dfc-ba14-dff87e1150cf)
- The query can be configured in the options.
- It can be used to better understand what object collisions are blocking a raycast.

### Command Bindings
Used to configure the command bindings. 
        
![Command Bindings](https://github.com/arnaud-jamin/Cog/assets/13844285/47179de9-ee28-427f-951f-5773b66cdeb2)
- Bindings are used to trigger console commands from a keyboard shortcuts, which are saved in `Saved/Config/WindowEditor/Input.ini` (in editor).
- Can be used to register the shortcuts Cog console commands:
  - `Cog.ToggleInput`
  - `Cog.LoadLayout <LayoutIndex>`
  - `Cog.ToggleSelectionMode`

### Debug Settings
Used to tweak how the debug display is drawn.

![Debug Settings](https://github.com/arnaud-jamin/Cog/assets/13844285/1c078311-2395-45e6-9a13-ddc405e1c914)

### Effects
Displays the gameplay effects of the selected actor.

![Effects](https://github.com/arnaud-jamin/Cog/assets/13844285/4fffa8fe-bd93-44bc-8ca5-27ae81f5da0c)
- Mouse over an effect to see its details such as its modifiers, the gameplay tags it grants, the remaining time, etc.

### Gamepad
Display the state of the gamepad

![Gamepad](https://github.com/arnaud-jamin/Cog/assets/13844285/734dd08a-5b9f-44cf-8d24-9bf257f08ec6)
- Can be displayed as an overlay
- Can be used to inject inputs to help debugging, as loosing window focus when breaking in the code doesn't affect the state of injected inputs unlike real inputs.

### Inspector
Used to inspect and modify an Object properties 

![Inspector](https://github.com/arnaud-jamin/Cog/assets/13844285/e6ed8841-40cc-4491-b46f-517e6cd7f296)
- Inspect the selected actor by default 
- Favorites can be added with a custom function defining what happens when the user press the Apply button (typically this can be used to modify and apply the Game User Settings)
- Manage a history of inspected objects.

### Log Categories
Can be used to activate and deactivate log categories

![Log Categories](https://github.com/arnaud-jamin/Cog/assets/13844285/b35ba24b-85e2-4264-8230-fb4a3899715b)
- Activating a log category set its verbosity to VeryVerbose.
- Deactivating a log category set its verbosity to Warning.
- The detailed verbosity of each log category can be shown by using the Option menu.
- A client can modify the verbosity of the server.
- The log categories are used to filter both the output log and the debug draw.

### Metric
Gather various values sent by the selected actor and compute their rate per second. This is typically used to compute the damage dealt or received per second.

![Metric](https://github.com/arnaud-jamin/Cog/assets/13844285/64d3cb7c-8731-4897-9ef9-b0868148ebe2)
- The following code shows how to add a metric:
```cpp
// Adding a metric
FCogDebugMetric::AddMetric(this, "Damage Dealt", MitigatedDamage, UnmitigatedDamage, false);
```

### Net Emulation
Used to configure the network emulation

![Net Emulation](https://github.com/arnaud-jamin/Cog/assets/13844285/97103f15-fae8-4fe9-8189-8fdbcab5cb20)

### Output Log
Display the output log based on each log categories verbosity.
    
![Output Log](https://github.com/arnaud-jamin/Cog/assets/13844285/71b1de06-a3d0-4e4d-83f3-c3f482c0d8f4)
- The verbosity of each log category can be configured in the 'Log Categories' window.

### Pools
Displays attributes of the selected actor as pools.

![Pools](https://github.com/arnaud-jamin/Cog/assets/13844285/7bb1aadd-9c0b-439f-b263-5ed842d0cd69)
- The pools can be configured in a data asset.

### Plots 
Plots values and events overtime. When applicable, only the values and events of the selected actor are displayed.

![Plots](https://github.com/arnaud-jamin/Cog/assets/13844285/def95b7b-ae59-4a8b-bc21-b07922e1fc6f)
- The following code shows how to plot values and events:
```cpp
    // Plotting a value
    FCogDebugPlot::PlotValue(this, "Speed", Velocity.Length());

    // Starting an event
    FCogDebugPlot::PlotEvent(this, "Effects", GameplayEffectSpec.Def->GetFName(), GameplayEffectSpec.GetDuration() == 0.0f)
                    .AddParam("Name", AbilitySystemComponent->CleanupName(GetNameSafe(GameplayEffectSpec.Def)))
                    .AddParam("Effect Instigator", GetNameSafe(GameplayEffectSpec.GetEffectContext().GetInstigator()))
                    .AddParam("Effect Level", GameplayEffectSpec.GetLevel())
                    .AddParam("Effect Duration", GameplayEffectSpec.GetDuration());

    // Stopping an event
    FCogDebugPlot::PlotEventStop(this, "Effects", RemovedGameplayEffect.Spec.Def->GetFName());
```

### Scalability
Used to configure the rendering quality.

![Scalability](https://github.com/arnaud-jamin/Cog/assets/13844285/4866b54c-5efa-4efa-a841-74ac8e1713c0)

### Skeleton
Display the bone hierarchy and the skeleton debug draw of the selected actor if it has a Skeletal Mesh.

![Skeleton](https://github.com/arnaud-jamin/Cog/assets/13844285/19648e3d-70dc-45bc-940d-e53eb9a99871)
- Mouse over a bone to highlight it.
- Right click a bone to access more debug display.
- Use the [Ctrl] key to toggle the bone debug draw recursively.
        
### Selection
Used to select an actor either by picking an actor in the world or by selecting an actor in the actor list.

![Selection](https://github.com/arnaud-jamin/Cog/assets/13844285/67cf9019-85c0-449f-88bf-0fc4841f8795)
- The actor list can be filtered by actor type (Actor, Character, etc).
- The current selection is used by various debug windows to filter out their content.

### Settings
Configure the settings of Cog Windows.

![Settings](https://github.com/arnaud-jamin/Cog/assets/13844285/edd6a701-9a50-41fa-b7df-90fd11cc2937)
- Can configure the input key to toggle the input focus between the game and imgui
- Can change the DPI Scaling

### Spawn
Used to spawn new actors in the world. The spawn list can be configured in a Data Asset.

![Spawn](https://github.com/arnaud-jamin/Cog/assets/13844285/b37f9307-5fd5-4ea5-9652-c265a8f63e32)

### Stats
Displays engine stats such as FPS, Ping, Packet Loss.

![Stats](https://github.com/arnaud-jamin/Cog/assets/13844285/e394ec7b-02fa-4b09-879b-90c82bd542ef)

### Tags
Displays the gameplay tags of the selected actor.

![Tags](https://github.com/arnaud-jamin/Cog/assets/13844285/3f14be3f-77f2-4d59-887d-1245fc97ed6a)

### Time Scale
Used to change the game global time scale. 

![Time Scale](https://github.com/arnaud-jamin/Cog/assets/13844285/d19198c5-37dd-400d-a09f-7a5077eb2511)
- If changed on a client the time scale is also modified on the game server, and replicated to the other clients.

### Transform
Used to read and set the selected actor transform. 

![Transform](https://github.com/arnaud-jamin/Cog/assets/13844285/f4fa2aeb-2e06-4820-8500-6eb14536b5b3)
- A gizmo in the viewport can also be used to change the transform.

![Transform](https://github.com/arnaud-jamin/Cog/assets/13844285/34657828-9fda-4af4-bdd0-4ea4514c23a0)

### Tweaks
Used to apply tweaks to all the spawned actors

![Tweaks](https://github.com/arnaud-jamin/Cog/assets/13844285/e8a5cdd5-908d-4ae3-901a-48addd4ce353)
- The tweaks are used to test various gameplay settings by actor category
- The tweaks can be configured in a data asset.

## Debug Functionalities

Cog provides C++ and Blueprint functions to log and debug draw within Log Categories. 

![Log Categories](https://github.com/arnaud-jamin/Cog/assets/13844285/cc0ba4a5-7fa6-44a8-b737-f523bdf94321)

Log and debug draw functions can be filtered by the selected actor. 

![Log and debug draw functions](https://github.com/arnaud-jamin/Cog/assets/13844285/a6f1329e-d49e-410f-8a70-4613aafabb5a)

## Setup

### Testing the sample

You must have Unreal 5.1 or greater and Visual Studio to launch the sample

1. Download the code
2. Right Click `Cog.uproject` and click `Generate Visual Studio project files`
3. Open Cog.sln
4. Select the `DebugGame Editor` or `Development Editor` solution configuration
5. Make sure `Cog` is set as the startup project
6. Start Debugging (F5)
7. Once in Unreal, press Play (Alt+P)
8. Press the `[F1]` key or use the `Cog.ToggleInput` console command to open the Imgui Main Menu.

### Integrating Cog in your project

The Cog repository has the following structure:
- `CogSample` - A Sample that demonstrate various Cog functionalities. The project was saved in Unreal 5.1
- `Plugins/CogAbility` - ImGui windows for the Gameplay Ability System (Abilities, Effects, Tags, ...)
- `Plugins/CogAI` - ImGui windows for AI (Behavior Tree, Blackboard)
- `Plugins/CogInput` - ImGui windows for the Enhanced Input library (Input action, Gamepad)
- `Plugins/Cog` - The main Cog plugin which contains the following modules
  - `CogEngine` - ImGui windows for the core unreal engine functionalities (Log, Stats, Time, Collisions, Skeleton, ...)
  - `CogWindow` - ImGui window management (Base Window, Layout)
  - `CogDebug` - Debug functionalities (Log, Debug Draw, Plot, Metric, ...)
  - `CogImGui` - Integration of Imgui for Unreal, inspired by [UnrealImGui](https://github.com/segross/UnrealImGui)
  - `CogCommon` - Interfaces implemented by your project actor classes which cannot be excluded from a shipping build
- `Plugins/CogAll` - Only contains a utility function to easily add all the built-in windows from all the Cog plugins. Useful for projects that do not need to exclude some plugins. 
  
Cog has multiple plugins to ease the integration for projects that do not use the `Ability System Component` or `Enhanced Input`. For the next steps, it is assumed all the plugins are used.

- Setup up module dependencies:
```c#
// CogSample.Build.cs
using UnrealBuildTool;

public class CogSample : ModuleRules
{
    public CogSample(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "CogCommon",  // CogCommon is required on all target configuration
            "AIModule",
            "Core",
            "CoreUObject", 
            "Engine", 
            "EnhancedInput",
            "GameplayTasks",
            "GameplayAbilities",
            "GameplayTags",
            "InputCore", 
            "NetCore",
        });

        // Other Cog plugins can be added only for specific target configuration
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "CogAbility",
                "CogAI",
                "CogAll",
                "CogDebug",
                "CogEngine",
                "CogImgui",
                "CogInput",
                "CogWindow",
            });
        }
    }
}
```


- In the class of your choice (in the sample we use the GameState class) add a reference to the CogWindowManager:
```cpp
// ACogSampleGameState.h
#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"
#include "GameFramework/GameStateBase.h"
#include "CogSampleGameState.generated.h"

class UCogWindowManager;

UCLASS()
class ACogSampleGameState : public AGameStateBase
{
    GENERATED_BODY()

    [...]

    // To make sure it doesn't get garbage collected.
    UPROPERTY()
    TObjectPtr<UObject> CogWindowManagerRef = nullptr;

#if ENABLE_COG
    TObjectPtr<UCogWindowManager> CogWindowManager = nullptr;
#endif //ENABLE_COG
};
```


- Instantiate the CogWindowManager and add some windows:
```cpp
// ACogSampleGameState.cpp
void ACogSampleGameState::BeginPlay()
{
    Super::BeginPlay();

#if ENABLE_COG
    CogWindowManager = NewObject<UCogWindowManager>(this);
    CogWindowManagerRef = CogWindowManager;

    // Add all the built-in windows
    Cog::AddAllWindows(*CogWindowManager);

    // Add a custom window
    CogWindowManager->AddWindow<FCogSampleWindow_Team>("Gameplay.Team");

    [...]
#endif //ENABLE_COG
}
```

- Tick the CogWindowManager:
```cpp

// ACogSampleGameState.cpp
ACogSampleGameState::ACogSampleGameState(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Enable ticking
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.SetTickFunctionEnable(true);
    PrimaryActorTick.bStartWithTickEnabled = true;
    
    [...]
}

void ACogSampleGameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

#if ENABLE_COG
    CogWindowManager->Tick(DeltaSeconds);
#endif //ENABLE_COG
}
```



- In your PlayerController class, spawn the Cog Replicators. The Replicator are used to communicate between the client and the server, for example to apply cheats, spawn actors, etc.
```cpp
void ACogSamplePlayerController::BeginPlay()
{
    Super::BeginPlay();

#if ENABLE_COG
    // Spawn the Replicator of each plugin
    ACogDebugReplicator::Spawn(this);
    ACogAbilityReplicator::Spawn(this);
    ACogEngineReplicator::Spawn(this);
#endif //ENABLE_COG
}
```

- Implement Cog Interfaces on your desired actor classes:
```cpp
// CogSampleCharacter.h
UCLASS(config=Game)
class ACogSampleCharacter : public ACharacter
    [...]
    , public ICogCommonDebugFilteredActorInterface
    , public ICogCommonAllegianceActorInterface
    [...]
```

```cpp
// CogSamplePlayerController.h
UCLASS(config=Game)
class ACogSamplePlayerController 
    : public APlayerController
    , public ICogCommonPossessorInterface
```

- In Unreal Editor create and configure the following Data Assets:
  - CogAbilityDataAsset
  - CogAIDataAsset
  - CogEngineDataAsset
  - CogInputDataAsset
  
![Data Assets](https://github.com/arnaud-jamin/Cog/assets/13844285/88384138-2ba7-43cf-a275-82f40503338e)

![Data Assets](https://github.com/arnaud-jamin/Cog/assets/13844285/cbda8065-c921-41a6-b06e-4302d8c72989)

![Data Assets](https://github.com/arnaud-jamin/Cog/assets/13844285/1f4f3255-4104-4dfc-ab9e-fd34335c0289)


