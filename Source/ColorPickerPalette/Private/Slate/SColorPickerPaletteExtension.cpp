// Copyright 2023 Yvidge. All Rights Reserved.


#include "Slate/SColorPickerPaletteExtension.h"

#include "ColorPickerPaletteUserSettings.h"
#include "ColorPickerPaletteUtils.h"
#include "SlateOptMacros.h"
#include "Framework/SlateDelegates.h"
#include "Slate/SColorPalette.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SColorPickerPaletteExtension::Construct(const FArguments& InArgs)
{
	OnColorChanged = InArgs._OnColorChanged;
	
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(450.f)
		.Padding(5.f)
		.MaxDesiredHeight(450.f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(PaletteWrapBox, SScrollBox)
			.Orientation(Orient_Vertical)
		]
	];

	GeneratePalettes();
}

void SColorPickerPaletteExtension::GeneratePalettes()
{
	PaletteWrapBox->ClearChildren();

	
	const auto& Palettes = UColorPickerPaletteUtils::GetRegisteredPalettes();
	
	if(Palettes.Num() == 0)
	{
		FSlateFontInfo Font = FCoreStyle::Get().GetFontStyle("Bold");
		Font.Size = 24;
		
		PaletteWrapBox->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(40.f, 0.f)
		[
			SNew(STextBlock)
			.Text(INVTEXT("Add color palettes in Project Settings"))
			.Font(Font)
			.AutoWrapText(true)
		];
		
		return;
	}
	
	for (auto& Palette : Palettes)
	{
		PaletteWrapBox->AddSlot()
		[
			SNew(SColorPalette)
			.ColorPaletteUID(Palette.Key)
			.ColorPalette(Palette.Value)
			.OnSelectColor(OnColorChanged)
			.ColorPaletteUserData(GetMutableDefault<UColorPickerPaletteUserSettings>()->GetPaletteUserData(Palette.Key))
		];
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
