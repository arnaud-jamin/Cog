using UnrealBuildTool;
using System.Collections.Generic;

public class CogSampleTarget : TargetRules
{
	public CogSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
		ExtraModuleNames.Add("CogSample");
        //bUseUnityBuild = false;
        //bUsePCHFiles = false;
    }
}
