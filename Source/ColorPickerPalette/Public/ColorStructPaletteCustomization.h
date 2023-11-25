// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Customizations/ColorStructCustomization.h"
#include "Engine/EngineTypes.h"

class SPaletteColorPicker;

/**
 * Color customization with warning icon and extended color picker 
 */
class COLORPICKERPALETTE_API FColorStructPaletteCustomization : public FColorStructCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual ~FColorStructPaletteCustomization() override;

protected:
	// Add warning icon to default header row
	virtual void MakeHeaderRow(TSharedRef<IPropertyHandle>& InStructPropertyHandle, FDetailWidgetRow& Row) override;

	// Same as CreateColorWidget, but we pass our callback to summon right color picker
	TSharedRef<SWidget> CreateColorBlock(TWeakPtr<IPropertyHandle> StructWeakHandlePtr);

	TSharedRef<SWidget> CreateWarningIcon();
	EVisibility GetWarnIconVisibility() const;

	virtual void OnSetColorFromColorPicker(FLinearColor NewColor);

	// Override color picker creation
	FReply OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void OnColorPickerWindowDeactivated();

	void CreateColorPickerWithPalette();
	virtual TSharedRef<SWidget> CreateCustomizationWidget(TSharedPtr<SWindow> Window);

	// Size used to adjust window summon location
	virtual FVector2D GetColorPickerDefaultWindowSize() const;


	void UpdateColor(FLinearColor NewColor);
	void UpdateWarning();

protected:
	TSharedPtr<SPaletteColorPicker> CreatedColorPicker;
	TSharedPtr<SImage> ColorWarnIcon;
	TWeakPtr<SWindow> ColorPickerWindow;

	FTimerHandle WindowDestroyTimer;
};
