using UnrealBuildTool;
using System.IO;

public class NetImgui : ModuleRules
{
    public NetImgui(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicSystemIncludePaths.Add(ModuleDirectory);
    }
}
