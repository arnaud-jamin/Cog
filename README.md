> [!IMPORTANT]
> The way Cog integrates in your project has been simplified. If you update Cog, please read the [integration guide](#Integrating-Cog-in-your-project).


# Cog
Cog is a set of debug tools for Unreal Engine built on top of [Dear ImGui](https://github.com/ocornut/imgui)

![Cog](https://github.com/arnaud-jamin/Cog/assets/13844285/21659aea-2cd8-4ef6-b3b0-5795f5f3246b)

- [Video](https://www.youtube.com/watch?v=ea5hz3cFcMM)
- [Sample Executable (Windows)](https://drive.google.com/file/d/1T7jQFoZ5rd6goBtDH-FCbjn6Kr1RzUCE/view?usp=sharing) (500 MB)

Cog provides:
- ImGui windows to inspect and configure various Unreal systems (Core Engine, Enhanced Inputs, Gameplay Abilities, AI)
- Window mangement with persistent configuration and layouts.
- C++ and Blueprint functions to log and debug draw within Log Categories.
- Control over the server regarding debug draw, logging, spawning, cheats.
- NetImgui support to ease the debugging of game server.

General Info:
- Cog can be used both in editor and package builds. It is disabled by default on shipping builds.
- Press the `[F1]` key or use the `Cog.ToggleInput` console command to open the ImGui Main Menu.
- Many windows display their contents based on a selected actor. The selector actor can be chosen using the `Engine/Selection` window or widget.
- Widgets such as Stats (FPS, Ping), Time Scale, Actor Selection, Console, can be added in the main menu bar from the 'Window/Widgets" menu.

## Cog Windows

### Abilities 

Displays the gameplay abilities of the selected actor.

![Abilities](https://github.com/user-attachments/assets/b332cf3a-9fee-408f-a86e-157c513c0ee2)
- Click the ability checkbox to force its activation or deactivation.
- Right click an ability to remove it, or open/close the ability separate window.
- Use the 'Give Ability' menu to manually give an ability from a list defined in a Data Asset.

---

### Actions
Displays the state of Input Action.

![Actions](https://github.com/user-attachments/assets/e4b60f69-efa0-4a23-b78e-9f261e5f78f5)
- Can be used to inject inputs to help debugging, as loosing window focus when breaking in the code doesn't affect the state of injected inputs unlike real inputs.

https://github.com/user-attachments/assets/2497900e-d2d9-4af9-abef-44f7f31c2726

---

### Attributes
Displays the gameplay attributes of the selected actor.

![Attributes](https://github.com/user-attachments/assets/ff010ac5-d8e5-44ca-b46f-263c45a0fc47)
- Attributes can be sorted by name, category or attribute set.
- Attributes with the Current value greater than the Base value are displayed in green.
- Attributes with the Current value lower than the Base value are displayed in red.
- Use the options 'Show Only Modified' to only show the attributes that have modifiers.

---

### Behavior Tree
Displays the behavior tree of the selected actor.

![Behavior Tree](https://github.com/arnaud-jamin/Cog/assets/13844285/c799e85f-b641-4d6f-9476-54a5cbd73c65)

---

### Blackboard 
Displays the blackboard of the selected actor.

![Blackboard](https://github.com/arnaud-jamin/Cog/assets/13844285/649d46d5-386c-4990-9f45-e4eb95a6b81a)

---

### Build Info 
Display the build information such as the build version, changelist, date, target, and so on.

![image](https://github.com/user-attachments/assets/b0383dea-a372-4ce2-94e2-6e1f9d3d6807)

---

### Cheats
Used to apply cheats to the selected actor.

![Cheats](https://github.com/user-attachments/assets/e3bbf157-904f-40e9-a0bf-e8152ff24af4)
- Cheats are replicated to the game server to be executed.
- Key modifiers can be used to change which actor should be affected by the cheat:
  - `[CTRL]` Apply the cheat to the controlled actor
  - `[ALT]` Apply the cheat to the allies of the selected actor
  - `[SHIFT]` Apply the cheat to the enemies of the selected actor
- Optionaly, cheats applied to the local player character are automatically reapplied.

![Cheats Settings](https://github.com/user-attachments/assets/006c4cf0-34e2-4e0d-8be7-be27db8796aa)

- Cheats can be defined in a Data Asset. Cog provide two default cheat execution (Apply Effect, Activate Ability).
- Cheat executions can be defined in blueprint.

![Cheats Asset](https://github.com/user-attachments/assets/46a14b89-ebab-4628-b97b-e5ccd2b63576)

---

### Collisions Tester
Used to test collision queries

![Collisions Tester](https://github.com/arnaud-jamin/Cog/assets/13844285/12f7cb7e-13c9-4eed-8ea4-f3eb7475cf34)
![Collisions Tester](https://github.com/arnaud-jamin/Cog/assets/13844285/4d01fd01-d0b3-41a7-9344-662f190ebaf0)
<video src="https://github.com/user-attachments/assets/95106f62-8a70-43ed-871d-c2f9380418a3" />

---

### Collisions Viewer
Used to inspect collisions in the world 

![Collisions Viewer](https://github.com/arnaud-jamin/Cog/assets/13844285/ccee855b-b33b-4dfc-ba14-dff87e1150cf)
- The query can be configured in the options.
- It can be used to better understand what object collisions are blocking a raycast.

---

### Command Bindings
Used to configure the command bindings. 
        
![Command Bindings](https://github.com/arnaud-jamin/Cog/assets/13844285/47179de9-ee28-427f-951f-5773b66cdeb2)
- Bindings are used to trigger console commands from a keyboard shortcuts, which are saved in `Saved/Config/WindowEditor/Input.ini` (in editor).

---

### Console
Used as a replacement of the Unreal console command.

![Console](https://github.com/user-attachments/assets/224b8913-61fb-4a63-b63a-fc3522fafe5f)
- The console exist as a window and a widget (placed in main menu bar).
- The console widget can optionaly get the keyboard focus when the main menu bar is shown.
- Use the Keys Up/Down or Tab/Shift-Tab to nagivate in the command list.
- The console settings can be change in the console window menu, or by right clicking the widget.

![Console Settings](https://github.com/user-attachments/assets/92bd2e0e-e860-403d-ade3-e780fb3a88c7)

---

### Debug Settings
Used to tweak how the debug display is drawn.

![Debug Settings](https://github.com/arnaud-jamin/Cog/assets/13844285/1c078311-2395-45e6-9a13-ddc405e1c914)

---

### Effects
Displays the gameplay effects of the selected actor.

![Effects](https://github.com/user-attachments/assets/ff9c0baa-80d8-4916-a81a-9914443bd88b)
- Mouse over an effect to see its details such as its modifiers, the gameplay tags it grants, the remaining time, etc.

---

### Gamepad
Display the state of the gamepad

![Gamepad](https://github.com/arnaud-jamin/Cog/assets/13844285/734dd08a-5b9f-44cf-8d24-9bf257f08ec6)
- Can be displayed as an overlay
- Can be used to inject inputs to help debugging, as loosing window focus when breaking in the code doesn't affect the state of injected inputs unlike real inputs.

---

### Inspector
Used to inspect and modify an Object properties 

![Inspector](https://github.com/arnaud-jamin/Cog/assets/13844285/e6ed8841-40cc-4491-b46f-517e6cd7f296)
- Inspect the selected actor by default 
- Favorites can be added with a custom function defining what happens when the user press the Apply button (typically this can be used to modify and apply the Game User Settings)
- Manage a history of inspected objects.

---

### Levels
Can be used to load specific levels

![image](https://github.com/user-attachments/assets/473ea0c8-05c6-4246-b295-995d4c43b6af)

---

### Log Categories
Can be used to activate and deactivate log categories

![Log Categories](https://github.com/arnaud-jamin/Cog/assets/13844285/b35ba24b-85e2-4264-8230-fb4a3899715b)
- Activating a log category set its verbosity to VeryVerbose.
- Deactivating a log category set its verbosity to Warning.
- The detailed verbosity of each log category can be shown by using the Option menu.
- A client can modify the verbosity of the server.
- The log categories are used to filter both the output log and the debug draw.

---

### Metric
Gather various values sent by the selected actor and compute their rate per second. This is typically used to compute the damage dealt or received per second.

![Metric](https://github.com/arnaud-jamin/Cog/assets/13844285/64d3cb7c-8731-4897-9ef9-b0868148ebe2)
- The following code shows how to add a metric:
```cpp
// Adding a metric
FCogDebugMetric::AddMetric(this, "Damage Dealt", MitigatedDamage, UnmitigatedDamage, false);
```

---

### Net Emulation
Used to configure the network emulation

![Net Emulation](https://github.com/arnaud-jamin/Cog/assets/13844285/97103f15-fae8-4fe9-8189-8fdbcab5cb20)

---

### NetImgui
Handle connections to a [NetImgui](https://github.com/sammyfreg/netImgui) server.

![NetImgui](https://github.com/user-attachments/assets/ea1d4a28-4c2f-460c-ac7d-5f6ef8e6e6dd)

The following image shows the editor running along a dedicated server. The NetImgui server displays the dedicated server imgui windows. 
This can be used to debug the state of the game server. For example the behavior trees are only available on the game server.
![image](https://github.com/user-attachments/assets/3cd788c9-9884-4c1c-8333-7a311bdcd20a)

---

### Notifications
Use to display and configure notifications

![Notification](https://github.com/user-attachments/assets/84db0359-f002-4c35-9d45-11187b3f9228)

- Notification can be added by using the following macros: `COG_NOTIFY`, `COG_NOTIFY_WARNING`, or `COG_NOTIFY_ERROR`.
- Notification settings can used to adjust the filtering, their location, their size, etc.

![Notification Settings](https://github.com/user-attachments/assets/aa47495e-4544-4e33-925e-d87cb9125e76)

---

### Output Log
Display the output log based on each log categories verbosity.
    
![Output Log](https://github.com/arnaud-jamin/Cog/assets/13844285/71b1de06-a3d0-4e4d-83f3-c3f482c0d8f4)
- The verbosity of each log category can be configured in the 'Log Categories' window.

---

### Pools
Displays attributes of the selected actor as pools.

![Pools](https://github.com/arnaud-jamin/Cog/assets/13844285/7bb1aadd-9c0b-439f-b263-5ed842d0cd69)
- The pools can be configured in a data asset.

---

### Plots 
Plots values and events overtime. When applicable, only the values and events of the selected actor are displayed.

![Plots](https://github.com/arnaud-jamin/Cog/assets/13844285/def95b7b-ae59-4a8b-bc21-b07922e1fc6f)
- The following code shows how to plot values and events:
```cpp
    // Plotting a value
    FCogDebug::Plot(this, "Speed", Velocity.Length());

    // Starting an event
    FCogDebug::StartEvent(this, "Effects", GameplayEffectSpec.Def->GetFName(), GameplayEffectSpec.GetDuration() == 0.0f)
                    .AddParam("Name", AbilitySystemComponent->CleanupName(GetNameSafe(GameplayEffectSpec.Def)))
                    .AddParam("Effect Instigator", GetNameSafe(GameplayEffectSpec.GetEffectContext().GetInstigator()))
                    .AddParam("Effect Level", GameplayEffectSpec.GetLevel())
                    .AddParam("Effect Duration", GameplayEffectSpec.GetDuration());

    // Stopping an event
    FCogDebug::StopEvent(this, "Effects", RemovedGameplayEffect.Spec.Def->GetFName());
```

---

### Scalability
Used to configure the rendering quality.

![Scalability](https://github.com/arnaud-jamin/Cog/assets/13844285/4866b54c-5efa-4efa-a841-74ac8e1713c0)
       
---

### Selection
Used to select an actor either by picking an actor in the world or by selecting an actor in the actor list.

![Selection](https://github.com/arnaud-jamin/Cog/assets/13844285/67cf9019-85c0-449f-88bf-0fc4841f8795)
- The actor list can be filtered by actor type (Actor, Character, etc).
- The current selection is used by various debug windows to filter out their content.

---

### Settings
Configure the settings of Cog.

![Settings](https://github.com/user-attachments/assets/6587d44d-bf40-4ff1-8d26-71f79a3110f2)
- Configure how ImGui behaves
- Change the visibility and ordeing of the widgets appearing in the main menu bar
- Change Cog Shorctuts
- Change the DPI Scaling. Use `[Ctrl][MouseWheel]` to change the DPI.

---

### Skeleton
Display the bone hierarchy and the skeleton debug draw of the selected actor if it has a Skeletal Mesh.

![Skeleton](https://github.com/arnaud-jamin/Cog/assets/13844285/19648e3d-70dc-45bc-940d-e53eb9a99871)
- Mouse over a bone to highlight it.
- Right click a bone to access more debug display.
- Use the `[Ctrl]` key to toggle the bone debug draw recursively.

---

### Spawn
Used to spawn new actors in the world. The spawn list can be configured in a Data Asset.

![Spawn](https://github.com/arnaud-jamin/Cog/assets/13844285/b37f9307-5fd5-4ea5-9652-c265a8f63e32)

---

### Stats
Displays engine stats such as FPS, Ping, Packet Loss.

![Stats](https://github.com/user-attachments/assets/424bac6a-76df-4d69-9d34-445a4a4bf536)

- Stats can be viewed as a widget in the main menu bar.
- Clicking on the stat povide a menu to force the stat to a reach a specific value.
- Right cliking the stat display the settings, where the specific stat values can be defined.

![Stats Settings](https://github.com/user-attachments/assets/a3545a8f-e742-4e8e-8758-03af133d932e)

---

### Tags
Displays the gameplay tags of the selected actor.

![Tags](https://github.com/user-attachments/assets/9aef871c-5af0-4ee0-8eb8-5fd706b43e51)

---

### Time Scale
Used to change the game global time scale. 

![Time Scale](https://github.com/user-attachments/assets/fc5a49e7-a04d-42b3-9d83-a8290e261665)
- If changed on a client the time scale is also modified on the game server, and replicated to the other clients.
- Time Scale can be viewed as a widget in the main menu bar.
- Right cliking the Time Scale display the settings, where the specific Time Scale values can be defined.

![image](https://github.com/user-attachments/assets/130f6250-af5b-41fb-8ae2-016c66fbdedc)

---

### Transform
Used to read and set the selected actor transform. 

![Transform](https://github.com/arnaud-jamin/Cog/assets/13844285/f4fa2aeb-2e06-4820-8500-6eb14536b5b3)
- A gizmo in the viewport can also be used to change the transform.

![Transform](https://github.com/arnaud-jamin/Cog/assets/13844285/34657828-9fda-4af4-bdd0-4ea4514c23a0)

---

### Tweaks
Used to apply tweaks to all the spawned actors

![Tweaks](https://github.com/arnaud-jamin/Cog/assets/13844285/e8a5cdd5-908d-4ae3-901a-48addd4ce353)
- The tweaks are used to test various gameplay settings by actor category
- The tweaks can be configured in a data asset.

---

## Debug Functionalities

Cog provides C++ and Blueprint functions to log and debug draw within Log Categories. 

![Log Categories](https://github.com/arnaud-jamin/Cog/assets/13844285/cc0ba4a5-7fa6-44a8-b737-f523bdf94321)

Log and debug draw functions can be filtered by the selected actor. 

![Log and debug draw functions](https://github.com/arnaud-jamin/Cog/assets/13844285/a6f1329e-d49e-410f-8a70-4613aafabb5a)

## Setup

### Testing the sample

You must have Unreal 5.5 or greater and Visual Studio to launch the sample

1. Download the code
2. Right Click `Cog.uproject` and click `Generate Visual Studio project files`
3. Open Cog.sln
4. Select the `DebugGame Editor` or `Development Editor` solution configuration
5. Make sure `Cog` is set as the startup project
6. Start Debugging (F5)
7. Once in Unreal, press Play (Alt+P)
8. Press the `[F1]` key or use the `Cog.ToggleInput` console command to open the Imgui Main Menu.

### Integrating Cog in your project

> [!IMPORTANT]
> The way Cog integrates in your project has been simplified. If you didn't modify Cog sources, you should remove all your existing Cog source files before updating. If you did modify Cog sources and stil have a `CogWindow` folder, consider deleting it before updating.

The Cog repository has the following structure:
- `CogSample` - A Sample that demonstrate various Cog functionalities. The project was saved in Unreal 5.5
- `Plugins/CogAbility` - ImGui windows for the Gameplay Ability System (Abilities, Effects, Tags, ...)
- `Plugins/CogAI` - ImGui windows for AI (Behavior Tree, Blackboard)
- `Plugins/CogInput` - ImGui windows for the Enhanced Input library (Input action, Gamepad)
- `Plugins/Cog` - The main Cog plugin which contains the following modules
  - `Cog` - Cog Subsystem (Manage windows, menu, settings, layout, ...)
  - `CogCommon` - Interfaces implemented by your project actor classes which cannot be excluded from a shipping build
  - `CogDebug` - Debug functionalities (Log, Debug Draw, Plot, Metric, ...)
  - `CogEngine` - ImGui windows for the core unreal engine functionalities (Log, Stats, Time, Collisions, Skeleton, ...)
  - `CogImGui` - Cog own integration of Imgui for Unreal, inspired by [UnrealImGui](https://github.com/segross/UnrealImGui)
- `Plugins/CogAll` - Only contains a utility function to easily add all the built-in windows from all the Cog plugins. Useful for projects that do not need to exclude some plugins. 
- `Plugins/CogCommonUI` - Contains an implementation of CommonUIActionRouterBase to let the Cog shorcuts work while in a CommonUI menu. Only use this plugin if you use CommonUI.
    
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
                "Cog",
                "CogAbility",
                "CogAI",
                "CogAll",
                "CogDebug",
                "CogEngine",
                "CogImgui",
                "CogInput",
            });
        }
    }
}
```

- Override the Unreal GameInstance:
```cpp
#pragma once

#include "CogSampleGameInstance.generated.h"

UCLASS()
class UCogSampleGameInstance : public UGameInstance
{
    GENERATED_BODY()

    void Init() override;
};
```

- Set your own game instance in the Unreal project settings:

![GameInstance](https://github.com/user-attachments/assets/070f98a2-6312-4110-9395-830bc01df369)

- In your GameInstance Init:
  - Add the windows you want to the CogSubsystem. In the sample we add all the built-in windows by calling Cog::AddAllWindows and we also add a custom window from the sample itself, showcasing that you can add your own windows.
  - Activate Cog

```cpp
#include "CogSampleGameInstance.h"

#include "CogCommon.h"

#if ENABLE_COG
#include "CogAll.h"
#include "CogSampleWindow_Team.h"
#include "CogSubsystem.h"
#endif 

void UCogSampleGameInstance::Init()
{
    Super::Init();

#if ENABLE_COG
    // Get the cog subsystem  
    if (UCogSubsystem* CogSubSystem = GetSubsystem<UCogSubsystem>())
    {
        // Add all the built-in windows. You copy paste this function code to organize the menu differently.
        Cog::AddAllWindows(*CogSubSystem);

        // Add a custom window
        CogSubSystem->AddWindow<FCogSampleWindow_Team>("Gameplay.Team");

        // Activate Cog
        CogSubSystem->Activate();
    }
#endif 
}
```

- Implement Cog Interfaces on your desired actor classes:
```cpp
// CogSampleCharacter.h
UCLASS(config=Game)
class ACogSampleCharacter : public ACharacter
    , public ICogCommonDebugFilteredActorInterface
    , public ICogCommonAllegianceActorInterface
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
  - CogEngineDataAsset
  
![Data Assets](https://github.com/user-attachments/assets/4dcbd8c6-e0de-44a0-bcb2-29c76f165029)

![Data Assets](https://github.com/user-attachments/assets/aea5a38e-fca4-44b7-b8e6-bee397ef5242)

![Data Assets](https://github.com/user-attachments/assets/7382b272-7561-45ca-9401-05107198657a)

- Reference the added Data Assets in the projet Asset Manager for them to be found in package mode:

![Data Assets](https://github.com/user-attachments/assets/39d0fcc2-1e82-4bb7-aa5e-0661ed9ab58b)




## Screenshots

Cog integrated in Lyra:
![Lyra](https://github.com/user-attachments/assets/cf432ac3-0fd5-4875-aa29-35161abd8dcc)
