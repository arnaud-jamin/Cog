using UnrealBuildTool;

public class CogDebugEditor : ModuleRules
{
	public CogDebugEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.Add(ModuleDirectory + "/Public");

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "AssetTools",
                "CogCommon",
                "CogDebug",
                "EditorStyle",
                "EditorWidgets",
				"InputCore",
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "SubobjectEditor",
                "ToolMenus",
                "UnrealEd",
                "BlueprintGraph",
                "GraphEditor",
            }
        );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
