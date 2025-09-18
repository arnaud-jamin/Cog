using UnrealBuildTool;
using System.Collections.Generic;

public class CogSampleTarget : TargetRules
{
	public CogSampleTarget(TargetInfo Target) : base(Target)
	{
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    }
}
