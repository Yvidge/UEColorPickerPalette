// Copyright 2023 Yvidge. All Rights Reserved.


#include "StrictColorStructCustomization.h"

#include "Slate/SColorPickerPaletteExtension.h"
#include "Slate/SPaletteColorPicker.h"
#include "Widgets/Images/SImage.h"


TSharedRef<IPropertyTypeCustomization> FStrictColorStructCustomization::MakeInstance()
{
	return MakeShareable(new FStrictColorStructCustomization());
}

void FStrictColorStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	FColorStructPaletteCustomization::CustomizeHeader(PropertyHandle, HeaderRow, CustomizationUtils);
	
	bIsLinearColor = true;
	ColorWarnIcon.Get()->SetVisibility(GetWarnIconVisibility());
}

TSharedRef<SWidget> FStrictColorStructCustomization::CreateCustomizationWidget(TSharedPtr<SWindow> Window)
{
	return SNew(SColorPickerPaletteExtension)
		.OnColorChanged(FOnLinearColorValueChanged::CreateSP(this, &FStrictColorStructCustomization::OnSetColorFromColorPicker));
}

void FStrictColorStructCustomization::OnSetColorFromColorPicker(FLinearColor NewColor)
{
	FColorStructPaletteCustomization::OnSetColorFromColorPicker(NewColor);
	ColorPickerWindow.Pin()->RequestDestroyWindow();
}

FVector2D FStrictColorStructCustomization::GetColorPickerDefaultWindowSize() const
{
	return FVector2D(500.f, 600.f);
}
