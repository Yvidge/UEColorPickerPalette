// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColorStructPaletteCustomization.h"

/**
 * Customization that will summon color picker with only registered colors
 */
class COLORPICKERPALETTE_API FStrictColorStructCustomization : public FColorStructPaletteCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
protected:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual TSharedRef<SWidget> CreateCustomizationWidget(TSharedPtr<SWindow> Window) override;

	void OnSetColorFromColorPicker(FLinearColor NewColor);

	virtual FVector2D GetColorPickerDefaultWindowSize() const override;
};
