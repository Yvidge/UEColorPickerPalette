// Copyright 2023 Yvidge. All Rights Reserved.


#include "Slate/SColorPaletteBlock.h"

#include "ColorPickerPaletteUserSettings.h"
#include "SlateOptMacros.h"
#include "Framework/SlateDelegates.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/SToolTip.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SColorPaletteBlock::Construct(const FArguments& InArgs)
{
	Color = InArgs._Color;
	OnSelectColor = InArgs._OnSelectColor;
	OptionalName = InArgs._OptionalName;
	bUseSRGB = InArgs._UseSRGB;
	
	const FSlateFontInfo SmallLayoutFont = FAppStyle::Get().GetFontStyle("Regular");
	const FSlateFontInfo SmallLabelFont = FAppStyle::Get().GetFontStyle("Bold");
	
	this->ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.HeightOverride(UColorPickerPaletteUserSettings::Get()->bUseCompactMode ? 35.f : 100.f)
			.WidthOverride(100.f)
			[
				
				SNew(SColorBlock)
				.Color(this, &SColorPaletteBlock::GetColor)
				.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Separate)
				.ColorIsHSV(false)
				.ShowBackgroundForAlpha(true)
				.UseSRGB(bUseSRGB)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(2)
		.HAlign(HAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(95.f)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &SColorPaletteBlock::GetColorFinalText)
				.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				.Clipping(EWidgetClipping::OnDemand)
			]
		]
		
	];
}

FReply SColorPaletteBlock::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnSelectColor.ExecuteIfBound(Color.Get());
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FText SColorPaletteBlock::GetColorFinalText() const
{
	return OptionalName.Get().IsEmpty() ? GetColorHexText() : GetOptionalName();
}

FText SColorPaletteBlock::GetOptionalName() const
{
	return OptionalName.Get();
}

FText SColorPaletteBlock::GetColorHexText() const
{
	return FText::FromString(TEXT("#") + Color.Get().ToFColor(true).ToHex());
}

FLinearColor SColorPaletteBlock::GetColor() const
{
	return Color.Get();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
