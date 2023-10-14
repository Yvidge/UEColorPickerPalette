// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColorStructPaletteCustomization.h"

class SPaletteColorPicker;
class FDetailWidgetRow;
class IPropertyHandle;
class IPropertyTypeCustomization;
class IPropertyTypeCustomizationUtils;

/**
 * Just like FSlateColorCustomization, but with warning icon and extended color picker
 */
class COLORPICKERPALETTE_API FSlateColorStructPaletteCustomization : public FColorStructPaletteCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:
	// IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils ) override;
	virtual void MakeHeaderRow(TSharedRef<class IPropertyHandle>& InStructPropertyHandle, FDetailWidgetRow& Row) override;

private:
	/** Called when the value is changed in the property editor. */
	virtual void OnValueChanged();

	ECheckBoxState GetForegroundCheckState() const;

	void HandleForegroundChanged(ECheckBoxState CheckedState);

private:
	/** slate color struct handle */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	TSharedPtr<IPropertyHandle> ColorRuleHandle;
	TSharedPtr<IPropertyHandle> SpecifiedColorHandle;
};
