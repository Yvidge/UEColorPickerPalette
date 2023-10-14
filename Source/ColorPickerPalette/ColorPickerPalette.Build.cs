// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ColorPickerPalette : ModuleRules
{
	public ColorPickerPalette(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"ColorPickerPaletteRuntime",
				"Core", 
				"DetailCustomizations", 
				"Slate",
				"AppFramework",
				"UnrealEd",
				"PropertyEditor",
				"InputCore",
				"DeveloperSettings",
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"SlateCore",
			}
		);
	}
}
