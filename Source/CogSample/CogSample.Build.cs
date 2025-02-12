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
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "InputCore", 
            "NetCore",
            "Niagara",
        });

        if (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Type != TargetRules.TargetType.Server)
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
                "Cog",
            });
        }
    }
}
