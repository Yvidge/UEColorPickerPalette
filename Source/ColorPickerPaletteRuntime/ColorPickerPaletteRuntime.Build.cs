// Copyright 2023 Yvidge. All Rights Reserved.

using UnrealBuildTool;

public class ColorPickerPaletteRuntime : ModuleRules
{
    public ColorPickerPaletteRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "DeveloperSettings"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject", "Engine"
            }
        );
    }
}