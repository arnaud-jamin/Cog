using UnrealBuildTool;
using System.IO;

public class ImGui : ModuleRules
{
    public ImGui(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicSystemIncludePaths.Add(ModuleDirectory);
    }
}
