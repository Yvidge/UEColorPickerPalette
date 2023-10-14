// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Colors/SColorPicker.h"

/**
 * 
 */
class COLORPICKERPALETTE_API SPaletteColorPicker : public SColorPicker
{
public:
	// This is a bit clunky, but we use this wrapper to take inner function from protected to public 
	void SetTargetColor(FLinearColor NewValue);
};
