using UnrealBuildTool;
using System.Collections.Generic;

public class CogSampleEditorTarget : TargetRules
{
	public CogSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		//IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
		ExtraModuleNames.Add("CogSample");
	}
}
