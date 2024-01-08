using UnrealBuildTool;

public class CogEngine : ModuleRules
{
    public CogEngine(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
	        new []
            {
                "CogDebug",
            });
        
        PrivateDependencyModuleNames.AddRange(
	        new []
            {
                "CogCommon",
                "CogImgui",
                "CogWindow",
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "NetCore",
                "Slate",
                "SlateCore",
            });
        
        if (Target.bBuildEditor)
        {
	        PrivateDependencyModuleNames.AddRange(
                new []
            {
	            "AssetTools"
            });
        }

    }
}
