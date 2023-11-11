// Copyright 2023 Yvidge. All Rights Reserved.

#include "ColorPickerPalette.h"

#include "ColorStructPaletteCustomization.h"
#include "SlateColorStructPaletteCustomization.h"
#include "StrictColorStructCustomization.h"

#define LOCTEXT_NAMESPACE "FColorPickerPaletteModule"

void FColorPickerPaletteModule::StartupModule()
{
	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomPropertyTypeLayout(NAME_LinearColor, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FColorStructPaletteCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(NAME_Color, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FColorStructPaletteCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("SlateColor", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSlateColorStructPaletteCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("StrictLinearColor", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStrictColorStructCustomization::MakeInstance));
	
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FColorPickerPaletteModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(NAME_LinearColor);
		PropertyModule.UnregisterCustomPropertyTypeLayout(NAME_Color);
		PropertyModule.UnregisterCustomPropertyTypeLayout("SlateColor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("StrictLinearColor");

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FColorPickerPaletteModule, ColorPickerPalette)