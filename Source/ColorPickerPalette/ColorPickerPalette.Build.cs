// Copyright 2023 Yvidge. All Rights Reserved.

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
				"EditorStyle"
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
