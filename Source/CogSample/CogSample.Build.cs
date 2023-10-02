using UnrealBuildTool;

public class CogSample : ModuleRules
{
	public CogSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "CogDebug",
            "Core", 
            "CoreUObject", 
            "Engine", 
            "EnhancedInput",
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
                "CogImgui",
                "CogInterfaces",
                "CogWindow",
                "CogEngine",
                "CogInput",
                "CogAbility",
            });
        }
    }
}
