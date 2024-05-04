// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColorPickerPaletteUserData.h"
#include "Engine/DeveloperSettings.h"
#include "ColorPickerPaletteUserSettings.generated.h"


UCLASS(Config=EditorPerProjectUserSettings, meta = (DisplayName="Color Picker User Settings"))
class COLORPICKERPALETTE_API UColorPickerPaletteUserSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UColorPickerPaletteUserSettings* Get();
	static UColorPickerPaletteUserSettings* GetMutable();

	virtual void PostCDOContruct() override;

	bool AreColorWarningsEnabled() const;
	bool IsColorInWhitelist(const FLinearColor& Color) const;
	bool IsColorInWhitelist(const FColor& Color, bool bSRGB) const;
	const FColorPickerPaletteUserData& GetPaletteUserData(const FName& PaletteUID) const;
	
	// Clear invalid ones and add missing entries for PalettesUserData config
	void RegenerateUserData();
	// Reconstruct Colors Data array from deprecated property
	void TryReconstuctColorsData();
	void SaveConfig();

public:
	/*
	 * When enabled, you will see color palettes extension in color picker 
	 * Disabling this will not affect how FStrictColor works
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	bool bEnableColorPickerExtension = true;
	
	/*
	 * When this property is set you will see warning icons in color properties
	 * Warning will appear if color is not registered via RegisteredPalettes or ColorWhitelist
	 * This is only cosmetic
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	bool bEnableColorWarnings = true;
	
	/*
	 * There is Whitelist for colors that you want neither to add to the palette nor to receive warnings about
	 * Add colors like white, black, purple, etc.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	TArray<FLinearColor> ColorWhitelist { FLinearColor::Black, FLinearColor::White, FLinearColor::Transparent, FLinearColor(1, 0, 1, 1)};

	/*
	 * Compact mode reduces the size of color blocks and palette names.
	 * It's recommended to enable this mode if you have more than 20 colors or more than 5 palettes
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "General")
	bool bUseCompactMode = false;

	
	UPROPERTY(Config)
	TMap<FName, FColorPickerPaletteUserData> PalettesUserData;
};