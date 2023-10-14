// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class COLORPICKERPALETTE_API SColorPaletteBlock : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SColorPaletteBlock)
	: _Color()
	, _OnSelectColor()
	, _UseSRGB()
		{ }

		/** The color this block uses */
		SLATE_ATTRIBUTE(FLinearColor, Color)
			
		/** Event called when this block is clicked */
		SLATE_EVENT(FOnLinearColorValueChanged, OnSelectColor)
			
		/** Whether to display sRGB color */
		SLATE_ATTRIBUTE(bool, UseSRGB)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

	FText GetColorHexText() const;
	FLinearColor GetColor() const;

protected:
	TAttribute<FLinearColor> Color;
	
	/** Event called when this block is clicked */
	FOnLinearColorValueChanged OnSelectColor;
	
	/** Whether to use display sRGB color */
	TAttribute<bool> bUseSRGB; 
};
