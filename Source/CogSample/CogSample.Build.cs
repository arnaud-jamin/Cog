using UnrealBuildTool;

public class CogSample : ModuleRules
{
	public CogSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "CogCommon",
            "Core",
            "CoreUObject", 
            "Engine", 
            "EnhancedInput",
            "GameplayTasks",
            "GameplayAbilities",
            "GameplayTags",
            "HeadMountedDisplay", 
            "InputCore", 
            "NetCore",
        });

        if (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Type != TargetRules.TargetType.Server)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "CogDebug",
                "CogImgui",
                "CogWindow",
                "CogEngine",
                "CogInput",
                "CogAbility",
            });
        }
    }
}
