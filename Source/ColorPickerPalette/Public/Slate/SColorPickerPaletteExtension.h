// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SScrollBox;
/**
 * 
 */
class COLORPICKERPALETTE_API SColorPickerPaletteExtension : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SColorPickerPaletteExtension)
		{ }
		SLATE_EVENT(FOnLinearColorValueChanged, OnColorChanged)
	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void GeneratePalettes();
	
protected:
	TSharedPtr<SScrollBox> PaletteWrapBox;

	FOnLinearColorValueChanged OnColorChanged;
};
