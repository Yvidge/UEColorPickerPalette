// Copyright 2023 Yvidge. All Rights Reserved.


#include "Slate/SColorPalette.h"

#include "ColorPickerPaletteDeveloperSettings.h"
#include "ColorPickerPaletteUserSettings.h"
#include "SlateOptMacros.h"
#include "Components/VerticalBox.h"
#include "Slate/SColorPaletteBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SColorPalette::Construct(const FArguments& InArgs)
{
	ColorPaletteUID = InArgs._ColorPaletteUID;
	ColorPalette = InArgs._ColorPalette;
	OnSelectColor = InArgs._OnSelectColor;
	ColorPaletteUserData = InArgs._ColorPaletteUserData;
	bCollapsed = ColorPaletteUserData.Get().bCollapsed;

	FSlateFontInfo NameFont = FCoreStyle::Get().GetFontStyle("Bold");
	NameFont.Size = UColorPickerPaletteUserSettings::Get()->bUseCompactMode ? 16 : 24;
	const FSlateFontInfo DescriptionFont = FCoreStyle::Get().GetFontStyle("Regular");

	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		
			// Palette Name
			+ SHorizontalBox::Slot()
			.Padding(0, 0, 0, 2)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(GetPaletteName())
				.Font(NameFont)
				.ColorAndOpacity(FLinearColor::White)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ExpanderArrow, SButton)
				.OnClicked(this, &SColorPalette::OnExpanderClicked)
				.ButtonStyle(FCoreStyle::Get(), "NoBorder")
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.ClickMethod(EButtonClickMethod::MouseDown)
				.ForegroundColor( FSlateColor::UseForeground() )
				.IsFocusable(false)
				[
					SNew(SImage)
					.Image(this, &SColorPalette::GetExpanderImage)
					.ColorAndOpacity( FSlateColor::UseForeground() )
				]
			]
		]
		
		// Palette Description
		+ SVerticalBox::Slot()
		.Padding(0, 0, 0, 2)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(GetPaletteDescription())
			.Font(DescriptionFont)
			.ColorAndOpacity(FLinearColor::Gray)
			.Visibility(ShouldShowDescription() ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed)
		]
		
		// Color Blocks
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 5)
		[
			SAssignNew(PaletteWrapBox, SWrapBox)
			.Orientation(Orient_Horizontal)
			.Visibility(this, &SColorPalette::GetWrapBoxVisibility)
			.UseAllottedSize(true)
		]
	];

	GenerateColorBlocks();
}

void SColorPalette::GenerateColorBlocks()
{
	PaletteWrapBox->ClearChildren();
	
	for (auto& Color : ColorPalette.Get().PaletteColors)
	{
		PaletteWrapBox->AddSlot()
		[
			SNew(SBox)
			.Padding(2.f)
			[
				SNew(SColorPaletteBlock)
				.Color(Color)
				.OnSelectColor(OnSelectColor)
				.UseSRGB(true)
			]
		];
	};
}

FReply SColorPalette::OnExpanderClicked()
{
	bCollapsed = !bCollapsed;
	PaletteWrapBox->SetVisibility(GetWrapBoxVisibility());
	GetMutableDefault<UColorPickerPaletteUserSettings>()->PalettesUserData.FindOrAdd(ColorPaletteUID.Get()).bCollapsed = bCollapsed;
	GetMutableDefault<UColorPickerPaletteUserSettings>()->SaveConfig();
	return FReply::Handled();
}

FText SColorPalette::GetPaletteName() const
{
	return ColorPalette.Get().PaletteName;
}

EVisibility SColorPalette::GetWrapBoxVisibility() const
{
	return bCollapsed ? EVisibility::Collapsed : EVisibility::Visible;
}

FText SColorPalette::GetPaletteDescription() const
{
	return ColorPalette.Get().PaletteDescription;
}

bool SColorPalette::ShouldShowDescription() const
{
	return !ColorPalette.Get().PaletteDescription.IsEmpty();
}

const FSlateBrush* SColorPalette::GetExpanderImage() const
{
	FName ResourceName;
	
	if (bCollapsed)
	{
		if (ExpanderArrow->IsHovered())
		{
			static const FName CollapsedHoveredName = "TreeArrow_Collapsed_Hovered";
			ResourceName = CollapsedHoveredName;
		}
		else
		{
			static const FName CollapsedName = "TreeArrow_Collapsed";
			ResourceName = CollapsedName;
		}
	}
	else
	{
		if (ExpanderArrow->IsHovered())
		{
			static const FName ExpandedHoveredName = "TreeArrow_Expanded_Hovered";
			ResourceName = ExpandedHoveredName;
		}
		else
		{
			static const FName ExpandedName = "TreeArrow_Expanded";
			ResourceName = ExpandedName;
		}
	}

	return FCoreStyle::Get().GetBrush(ResourceName);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
