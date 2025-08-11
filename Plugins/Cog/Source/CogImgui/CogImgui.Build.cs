using UnrealBuildTool;
using System.IO;

public class CogImgui : ModuleRules
{
	public CogImgui(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "ImGui",
            "ImPlot",
            "NetImgui",
			"CogCommon",
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "ApplicationCore",
            "CoreUObject",
            "Engine",
            "InputCore",
            "RenderCore",
            "Slate",
            "SlateCore",
            "Sockets",
        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new[]
            {
                "MainFrame",
                "EditorStyle",
                "Settings",
                "UnrealEd",
            });
        }

        PublicDefinitions.Add("IMGUI_USER_CONFIG=\"CogImGuiConfig.h\"");
        PublicDefinitions.Add("IMGUI_API=COGIMGUI_API");
        PublicDefinitions.Add("IMPLOT_API=COGIMGUI_API");
    }
}
