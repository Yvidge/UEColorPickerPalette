// Copyright 2023 Yvidge. All Rights Reserved.


#include "SlateColorStructPaletteCustomization.h"

#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Attribute.h"
#include "Misc/Optional.h"
#include "PropertyHandle.h"
#include "SlotBase.h"
#include "UObject/NameTypes.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

class IPropertyTypeCustomization;
class IPropertyTypeCustomizationUtils;

TSharedRef<IPropertyTypeCustomization> FSlateColorStructPaletteCustomization::MakeInstance()
{
	return MakeShareable(new FSlateColorStructPaletteCustomization());
}

void FSlateColorStructPaletteCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle,
                                                            class FDetailWidgetRow& InHeaderRow,
                                                            IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName ColorUseRuleKey(TEXT("ColorUseRule"));
	static const FName SpecifiedColorKey(TEXT("SpecifiedColor"));

	StructPropertyHandle = InStructPropertyHandle;

	ColorRuleHandle = InStructPropertyHandle->GetChildHandle(ColorUseRuleKey);
	SpecifiedColorHandle = InStructPropertyHandle->GetChildHandle(SpecifiedColorKey);

	check(ColorRuleHandle.IsValid());
	check(SpecifiedColorHandle.IsValid());

	ColorRuleHandle->MarkHiddenByCustomization();
	SpecifiedColorHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(SharedThis(this), &FSlateColorStructPaletteCustomization::OnValueChanged));

	FColorStructPaletteCustomization::CustomizeHeader(SpecifiedColorHandle.ToSharedRef(), InHeaderRow,
	                                                  StructCustomizationUtils);

	// Slate brushes always default to sRGB mode.
	sRGBOverride = true;
}

void FSlateColorStructPaletteCustomization::MakeHeaderRow(TSharedRef<class IPropertyHandle>& InStructPropertyHandle,
                                                          FDetailWidgetRow& Row)
{
	// NOTE: Ignore InStructPropertyHandle, it's going to be the specified color handle that we passed to the color customization base class.

	Row
		.NameContent()
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(StructPropertyHandle->GetPropertyDisplayName())
			.ToolTipText(StructPropertyHandle->GetToolTipText())
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)

			// Default slate color row
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(250.f)
				[

					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						CreateColorBlock(StructPropertyHandle)
					]

					+ SHorizontalBox::Slot()
					  .AutoWidth()
					  .Padding(2.f, 0.f, 0.f, 0.f)
					[
						SNew(SCheckBox)
						.IsChecked(this, &FSlateColorStructPaletteCustomization::GetForegroundCheckState)
						.OnCheckStateChanged(this, &FSlateColorStructPaletteCustomization::HandleForegroundChanged)
						[
							SNew(STextBlock)
							.Font(IDetailLayoutBuilder::GetDetailFont())
							.Text(NSLOCTEXT("SlateColorCustomization", "Inherit", "Inherit"))
							.ToolTipText(NSLOCTEXT("SlateColorCustomization", "InheritToolTip",
							                       "Uses the foreground color inherited down the widget hierarchy"))
						]
					]
				]
			]

			// Warning icon
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .VAlign(VAlign_Center)
			  .Padding(10.f, 0.f, 0.f, 0.f)
			[
				CreateWarningIcon()
			]
		];
	
	StructPropertyHandle->SetOnPropertyResetToDefault(FSimpleDelegate::CreateRaw(this, &FSlateColorStructPaletteCustomization::UpdateWarning));
	StructPropertyHandle->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FSlateColorStructPaletteCustomization::UpdateWarning));
}

void FSlateColorStructPaletteCustomization::OnValueChanged()
{
	ColorRuleHandle->SetValueFromFormattedString(TEXT("UseColor_Specified"));
}

ECheckBoxState FSlateColorStructPaletteCustomization::GetForegroundCheckState() const
{
	FString ColorRuleValue;
	ColorRuleHandle->GetValueAsFormattedString(ColorRuleValue);

	if (ColorRuleValue == TEXT("UseColor_Foreground"))
	{
		return ECheckBoxState::Checked;
	}

	return ECheckBoxState::Unchecked;
}

void FSlateColorStructPaletteCustomization::HandleForegroundChanged(ECheckBoxState CheckedState)
{
	if (CheckedState == ECheckBoxState::Checked)
	{
		ColorRuleHandle->SetValueFromFormattedString(TEXT("UseColor_Foreground"));
	}
	else
	{
		ColorRuleHandle->SetValueFromFormattedString(TEXT("UseColor_Specified"));
	}
}
