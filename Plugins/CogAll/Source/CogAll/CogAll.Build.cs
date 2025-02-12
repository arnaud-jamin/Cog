using UnrealBuildTool;

public class CogAll : ModuleRules
{
    public CogAll(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
                "CoreUObject",
                "CogCommon",
                "CogAbility",
                "CogAI",
                "CogEngine",
                "CogInput",
                "Cog",
                "Engine",
            }
            );
    }
}
