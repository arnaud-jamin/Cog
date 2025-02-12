using UnrealBuildTool;

public class Cog : ModuleRules
{
	public Cog(ReadOnlyTargetRules Target) : base(Target)
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
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "NetCore",
            }
			);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"AssetTools"
            });
		}


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
