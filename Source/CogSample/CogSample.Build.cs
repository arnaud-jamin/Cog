using UnrealBuildTool;

public class CogSample : ModuleRules
{
	public CogSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "AIModule",
            "CogCommon",
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

        if (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Type != TargetRules.TargetType.Server)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "CogDebug",
                "CogImgui",
                "CogWindow",
                "CogEngine",
                "CogAbility",
                "CogAI",
                "CogInput",
            });
        }
    }
}
