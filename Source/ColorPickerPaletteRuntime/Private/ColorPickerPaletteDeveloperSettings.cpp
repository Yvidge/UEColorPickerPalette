// Copyright 2023 Yvidge. All Rights Reserved.


#include "ColorPickerPaletteDeveloperSettings.h"


const UColorPickerPaletteDeveloperSettings* UColorPickerPaletteDeveloperSettings::Get()
{
	return GetDefault<UColorPickerPaletteDeveloperSettings>();
}

UColorPickerPaletteDeveloperSettings* UColorPickerPaletteDeveloperSettings::GetMutable()
{
	return GetMutableDefault<UColorPickerPaletteDeveloperSettings>();
}
