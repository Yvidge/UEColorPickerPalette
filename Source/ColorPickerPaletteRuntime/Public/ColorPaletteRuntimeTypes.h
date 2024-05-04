// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "ColorPaletteRuntimeTypes.generated.h"

USTRUCT(BlueprintType)
struct FColorPaletteColorData
{
	GENERATED_BODY()

	FColorPaletteColorData() { }
	FColorPaletteColorData(FLinearColor Color) : Color(Color) { }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FText OptionalName;
};


USTRUCT(BlueprintType)
struct FColorPalette
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FText PaletteName = FText::FromString(TEXT("Default Palette"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FText PaletteDescription;

	UPROPERTY(meta=(DeprecatedProperty, DeprecationMessage="PaletteColors are deprecated. Use PaletteColorsData instead"))
	TArray<FLinearColor> PaletteColors_DEPRECATED;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	TArray<FColorPaletteColorData> PaletteColorsData;
};

/**
 * Empty wrapper for linear color that will summon color picker with only registered colors.
 * See FStrictColorStructCustomization for more info.
 * Otherwise, it's common linear color
 */
USTRUCT(BlueprintType)
struct FStrictLinearColor : public FLinearColor
{
	GENERATED_BODY()
};
