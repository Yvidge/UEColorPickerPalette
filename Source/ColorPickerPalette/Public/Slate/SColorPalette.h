// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColorPaletteRuntimeTypes.h"
#include "ColorPickerPaletteUserData.h"
#include "Widgets/SCompoundWidget.h"

class SWrapBox;
/**
 * 
 */
class COLORPICKERPALETTE_API SColorPalette : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SColorPalette)
	{ }
		/** Color Palette UID */
		SLATE_ATTRIBUTE(FName, ColorPaletteUID)
	
		/** Color Palette to initialize from */
		SLATE_ATTRIBUTE(FColorPalette, ColorPalette)
	
		/** Event called when this block is clicked */
		SLATE_EVENT(FOnLinearColorValueChanged, OnSelectColor)

		/** Per user state data */
		SLATE_ATTRIBUTE(FColorPickerPaletteUserData, ColorPaletteUserData)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	void GenerateColorBlocks();

	FReply OnExpanderClicked();
	
	FText GetPaletteName() const;
	EVisibility GetWrapBoxVisibility() const;
	FText GetPaletteDescription() const;
	bool ShouldShowDescription() const;
	const FSlateBrush* GetExpanderImage() const;
	
protected:
	bool bCollapsed = false;
	TSharedPtr<SButton> ExpanderArrow;
	TSharedPtr<SWrapBox> PaletteWrapBox;

	TAttribute<FName> ColorPaletteUID;
	TAttribute<FColorPalette> ColorPalette;
	FOnLinearColorValueChanged OnSelectColor;
	TAttribute<FColorPickerPaletteUserData> ColorPaletteUserData;
};
