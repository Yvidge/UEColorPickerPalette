// Copyright 2023 Yvidge. All Rights Reserved.


#include "ColorStructPaletteCustomization.h"

#include "ColorPickerPaletteDeveloperSettings.h"
#include "ColorPickerPaletteUserSettings.h"
#include "ColorPickerPaletteUtils.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Editor.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SColorPickerPaletteExtension.h"
#include "Slate/SPaletteColorPicker.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "EditorStyleSet.h"


TSharedRef<IPropertyTypeCustomization> FColorStructPaletteCustomization::MakeInstance()
{
	return  MakeShareable(new FColorStructPaletteCustomization);
}

FColorStructPaletteCustomization::~FColorStructPaletteCustomization()
{
	if(GWorld)
	{
		GWorld->GetTimerManager().ClearTimer(WindowDestroyTimer);
	}
	
	if(ColorPickerWindow.IsValid())
	{
		ColorPickerWindow.Pin()->RequestDestroyWindow();
	}
}

void FColorStructPaletteCustomization::MakeHeaderRow(TSharedRef<IPropertyHandle>& InStructPropertyHandle, FDetailWidgetRow& Row)
{
	TSharedPtr<SWidget> ColorWidget;
	float ContentWidth = 250.0f;

	TWeakPtr<IPropertyHandle> StructWeakHandlePtr = InStructPropertyHandle;
	
	StructWeakHandlePtr.Pin()->SetOnPropertyResetToDefault(FSimpleDelegate::CreateRaw(this, &FColorStructPaletteCustomization::UpdateWarning));
	StructWeakHandlePtr.Pin()->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FColorStructPaletteCustomization::UpdateWarning));
	
	if (InStructPropertyHandle->HasMetaData("InlineColorPicker"))
	{
		ColorWidget = CreateInlineColorPicker(StructWeakHandlePtr);
		ContentWidth = 384.0f;
	}
	else
	{
		ColorWidget = CreateColorBlock(StructWeakHandlePtr);
	}

	Row.NameContent()
	[
		InStructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(ContentWidth)
	[
		SNew(SHorizontalBox)

		// Color block
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			ColorWidget.ToSharedRef()
		]

		// Warning icon
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.f, 0.f, 0.f, 0.f)
		.VAlign(VAlign_Center)
		[
			CreateWarningIcon()
		]
	];
}

TSharedRef<SWidget> FColorStructPaletteCustomization::CreateColorBlock(TWeakPtr<IPropertyHandle> StructWeakHandlePtr)
{
	FSlateFontInfo NormalText = IDetailLayoutBuilder::GetDetailFont();

	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 2.0f)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			[
				// Displays the color with alpha unless it is ignored
				SAssignNew(ColorPickerParentWidget, SColorBlock)
				.Color(this, &FColorStructPaletteCustomization::OnGetColorForColorBlock)
				.ShowBackgroundForAlpha(true)
				.IgnoreAlpha(bIgnoreAlpha)
				.OnMouseButtonDown(this, &FColorStructPaletteCustomization::OnMouseButtonDownColorBlock)
				.Size(FVector2D(35.0f, 12.0f))
				.IsEnabled(this, &FColorStructPaletteCustomization::IsValueEnabled, StructWeakHandlePtr)
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("PropertyEditor", "MultipleValues", "Multiple Values"))
				.Font(NormalText)
				.ColorAndOpacity(FSlateColor(FLinearColor::Black)) // we know the background is always white, so can safely set this to black
				.Visibility(this, &FColorStructPaletteCustomization::GetMultipleValuesTextVisibility)
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 2.0f)
		[
			// Displays the color without alpha
			SNew(SColorBlock)
			.Color(this, &FColorStructPaletteCustomization::OnGetColorForColorBlock)
			.ShowBackgroundForAlpha(false)
			.IgnoreAlpha(true)
			.OnMouseButtonDown(this, &FColorStructPaletteCustomization::OnMouseButtonDownColorBlock)
			.Size(FVector2D(35.0f, 12.0f))
		];
}

TSharedRef<SWidget> FColorStructPaletteCustomization::CreateWarningIcon()
{
	return
		SNew(SBox)
		.MinDesiredHeight(20.f)
		.MinDesiredWidth(20.f)
		[
			SAssignNew(ColorWarnIcon, SImage)
			.Image(FEditorStyle::GetBrush("Icons.Error"))
			.ToolTipText(INVTEXT("Warning: This color is not registered in project settings. \nCheck ColorPickerPaletteSettings \nYou can disable this warning or setup whitelist in Editor Preferences"))
			.Visibility(GetWarnIconVisibility())
		];
}

TSharedRef<SWidget> FColorStructPaletteCustomization::CreateCustomizationWidget(TSharedPtr<SWindow> Window)
{
	const bool bRefreshOnlyOnOk = bDontUpdateWhileEditing || StructPropertyHandle->HasMetaData("DontUpdateWhileEditing");
	const bool bOnlyRefreshOnMouseUp = StructPropertyHandle->HasMetaData("OnlyUpdateOnInteractionEnd");
	
	FLinearColor InitialColor;
	GetColorAsLinear(InitialColor);
	
	CreatedColorPicker = SNew(SPaletteColorPicker)
		.DisplayInlineVersion(false)
		.OnlyRefreshOnMouseUp(bOnlyRefreshOnMouseUp)
		.OnlyRefreshOnOk(bRefreshOnlyOnOk)
		.DisplayGamma(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma)))
		.OnColorCommitted(FOnLinearColorValueChanged::CreateSP(this, &FColorStructPaletteCustomization::OnSetColorFromColorPicker))
		.OnColorPickerCancelled(FOnColorPickerCancelled::CreateSP(this, &FColorStructPaletteCustomization::OnColorPickerCancelled))
		.OnColorPickerWindowClosed(FOnWindowClosed::CreateSP(this, &FColorStructPaletteCustomization::OnColorPickerWindowClosed))
		.OnInteractivePickBegin(FSimpleDelegate::CreateSP(this, &FColorStructPaletteCustomization::OnColorPickerInteractiveBegin))
		.OnInteractivePickEnd(FSimpleDelegate::CreateSP(this, &FColorStructPaletteCustomization::OnColorPickerInteractiveEnd))
		.sRGBOverride(sRGBOverride)
		.TargetColorAttribute(InitialColor)
		.ExpandAdvancedSection(true)
		.ParentWindow(Window)
		.OptionalOwningDetailsView(ColorPickerParentWidget);
	
	if(!UColorPickerPaletteUserSettings::Get()->bEnableColorPickerExtension)
	{
		return CreatedColorPicker.ToSharedRef();
	}
	
	return SNew(SHorizontalBox)
	
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SColorPickerPaletteExtension)
		.OnColorChanged(FOnLinearColorValueChanged::CreateSP(this, &FColorStructPaletteCustomization::UpdateColor))
	]
	
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		CreatedColorPicker.ToSharedRef()
	];
}

FVector2D FColorStructPaletteCustomization::GetColorPickerDefaultWindowSize() const
{
	return SColorPicker::DEFAULT_WINDOW_SIZE + FVector2D(500.f, 100.f);
}

void FColorStructPaletteCustomization::OnSetColorFromColorPicker(FLinearColor NewColor)
{
	FColorStructCustomization::OnSetColorFromColorPicker(NewColor);
	ColorWarnIcon.Get()->SetVisibility(GetWarnIconVisibility());
}

FReply FColorStructPaletteCustomization::OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}
	
	bool CanShowColorPicker = true;
	if (StructPropertyHandle.IsValid() && StructPropertyHandle->GetProperty() != nullptr)
	{
		CanShowColorPicker = !StructPropertyHandle->IsEditConst();
	}
	if (CanShowColorPicker)
	{
		CreateColorPickerWithPalette();
	}

	return FReply::Handled();
}

void FColorStructPaletteCustomization::OnColorPickerWindowDeactivated()
{
	if(this)
	{
		if(GWorld)
		{
			FTimerDelegate Delegate;
			Delegate.BindLambda([this]()
			{
				if(ColorPickerWindow.IsValid())
				{
					ColorPickerWindow.Pin()->RequestDestroyWindow();
					ColorPickerWindow.Reset();
				}
			});
			GWorld->GetTimerManager().SetTimer(WindowDestroyTimer, Delegate, 0.1f, false);
		}
	}
}

void FColorStructPaletteCustomization::CreateColorPickerWithPalette()
{
	GEditor->BeginTransaction(FText::Format(INVTEXT("Edit {0}"), StructPropertyHandle->GetPropertyDisplayName()));

	SavedPreColorPickerColors.Empty();
	TArray<FString> PerObjectValues;
	StructPropertyHandle->GetPerObjectValues(PerObjectValues);

	for (int32 ObjectIndex = 0; ObjectIndex < PerObjectValues.Num(); ++ObjectIndex)
	{
		if (bIsLinearColor)
		{
			FLinearColor Color;
			Color.InitFromString(PerObjectValues[ObjectIndex]);
			SavedPreColorPickerColors.Add(FLinearOrSrgbColor(Color));
		}
		else
		{
			FColor Color;
			Color.InitFromString(PerObjectValues[ObjectIndex]);
			SavedPreColorPickerColors.Add(FLinearOrSrgbColor(Color));
		}
	}
	
	// Determine the position of the window so that it will spawn near the mouse, but not go off the screen.
	FVector2D CursorPos = FSlateApplication::Get().GetCursorPos();
	FSlateRect Anchor(CursorPos.X, CursorPos.Y, CursorPos.X, CursorPos.Y);
	FVector2D AdjustedSummonLocation = FSlateApplication::Get().CalculatePopupWindowPosition(
		Anchor, GetColorPickerDefaultWindowSize(), true, FVector2D::ZeroVector, Orient_Horizontal);

	
	TSharedRef<SBorder> WindowContent = SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(FMargin(8.0f, 8.0f));
	
	TSharedPtr<SWindow> Window = nullptr;

	SAssignNew(Window, SWindow)
	.AutoCenter(EAutoCenter::None)
	.ScreenPosition(AdjustedSummonLocation)
	.SupportsMaximize(false)
	.SupportsMinimize(false)
	.SizingRule(ESizingRule::Autosized)
	.Title(INVTEXT("Color Picker"))
	[
		WindowContent
	];

	if(GWorld)
	{
		GWorld->GetTimerManager().ClearTimer(WindowDestroyTimer);
	}
	
	Window.Get()->GetOnWindowDeactivatedEvent().AddRaw(this, &FColorStructPaletteCustomization::OnColorPickerWindowDeactivated);
	
	WindowContent->SetContent(CreateCustomizationWidget(Window));
	
	FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	
	ColorPickerWindow = Window;
}

EVisibility FColorStructPaletteCustomization::GetWarnIconVisibility() const
{
	if(UColorPickerPaletteUserSettings::Get()->AreColorWarningsEnabled())
	{
		bool bRegistered;
		const FLinearColor LinearColor = OnGetColorForColorBlock();
		if(bIsLinearColor)
		{
			bRegistered = UColorPickerPaletteUtils::IsLinearColorRegistered(LinearColor) || UColorPickerPaletteUserSettings::Get()->IsColorInWhitelist(LinearColor); 
		}
		else
		{
			const FColor Color = LinearColor.ToFColor(true);
			bRegistered = UColorPickerPaletteUtils::IsColorRegistered(Color) || UColorPickerPaletteUserSettings::Get()->IsColorInWhitelist(Color, true); 
		}
		
		return bRegistered ? EVisibility::Collapsed : EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}

void FColorStructPaletteCustomization::UpdateColor(FLinearColor NewColor)
{
	CreatedColorPicker->SetTargetColor(NewColor);
}

void FColorStructPaletteCustomization::UpdateWarning()
{
	ColorWarnIcon.Get()->SetVisibility(GetWarnIconVisibility());
}
