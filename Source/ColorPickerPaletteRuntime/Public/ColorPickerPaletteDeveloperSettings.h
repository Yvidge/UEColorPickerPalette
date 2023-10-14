// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColorPaletteRuntimeTypes.h"
#include "Engine/DeveloperSettings.h"
#include "ColorPickerPaletteDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Color Picker Palette Settings"))
class COLORPICKERPALETTERUNTIME_API UColorPickerPaletteDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UColorPickerPaletteDeveloperSettings* Get();

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	TMap<FName, FColorPalette> RegisteredPalettes;
};
