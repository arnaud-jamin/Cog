using UnrealBuildTool;

public class CogDebug : ModuleRules
{
    public CogDebug(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Engine",
                "Core",
                "CogImgui",
                "CogCommon", 
            }
            );
            
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Slate",
                "SlateCore",
                "NetCore",
            }
            );
    }
}
