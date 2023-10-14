// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "ColorPaletteRuntimeTypes.generated.h"


USTRUCT(BlueprintType)
struct FColorPalette
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FText PaletteName = FText::FromString(TEXT("Default Palette"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FText PaletteDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	TArray<FLinearColor> PaletteColors;
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
