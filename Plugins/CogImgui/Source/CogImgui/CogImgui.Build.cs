using UnrealBuildTool;
using System.IO;

public class CogImgui : ModuleRules
{
	public CogImgui(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;

        PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "../ThirdParty/imgui/"),
                Path.Combine(ModuleDirectory, "../ThirdParty/implot/"),
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "ThirdParty/imgui",
                "ThirdParty/implot",
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Projects"
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "InputCore",
				"Slate",
                "SlateCore",
			}
			);

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "EditorStyle",
                    "Settings",
                    "UnrealEd",
                }
                );
        }
    }
}
