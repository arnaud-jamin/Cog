using UnrealBuildTool;
using System.IO;

public class ImPlot : ModuleRules
{
    public ImPlot(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicSystemIncludePaths.Add(ModuleDirectory);
    }
}
