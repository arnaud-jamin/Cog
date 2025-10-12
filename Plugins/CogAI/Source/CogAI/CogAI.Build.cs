using UnrealBuildTool;

public class CogAI : ModuleRules
{
    public CogAI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
            }
            );
                
        
        PrivateIncludePaths.AddRange(
            new string[] {
            }
            );
            
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CogCommon",
                "CogImgui",
                "CogDebug",
                "Cog",
                "AIModule",
            }
            );
            
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
            }
            );
        
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
            );
    }
}
