using UnrealBuildTool;

public class CogCommon : ModuleRules
{
	public CogCommon(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseUnity = false;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		PublicIncludePathModuleNames.AddRange(
			new string[]
			{
				"CogDebug"
			}
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
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
