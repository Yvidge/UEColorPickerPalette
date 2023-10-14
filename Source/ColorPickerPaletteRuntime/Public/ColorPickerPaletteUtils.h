// Copyright Yvidge. All right reserved  

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ColorPaletteRuntimeTypes.h"
#include "ColorPickerPaletteUtils.generated.h"


UCLASS()
class COLORPICKERPALETTERUNTIME_API UColorPickerPaletteUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Checks if input linear color is included in any registered palette
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ColorPickerPalette, meta=(AutoCreateRefTerm="Color"))
	static bool IsLinearColorRegistered(const FLinearColor& Color);

	/**
	 * Checks if input color is included in any registered palette
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ColorPickerPalette)
	static bool IsColorRegistered(const FColor& Color, bool bSRGB = true);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ColorPickerPalette)
	static TMap<FName, FColorPalette> GetRegisteredPalettes();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ColorPickerPalette, meta=(AutoCreateRefTerm="PaletteUID"))
	static FColorPalette GetColorPaletteByUID(const FName& PaletteUID);
};
