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
	float ContentWidth = 125.0f;

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
		.AutoWidth()
		[
			ColorWidget.ToSharedRef()
		]

		// Warning icon
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			CreateWarningIcon()
		]
	];
}

TSharedRef<SWidget> FColorStructPaletteCustomization::CreateColorBlock(
	TWeakPtr<IPropertyHandle> StructWeakHandlePtr)
{
	return
		SNew(SBox)
		.Padding(FMargin(0.f,0.f,4.0f,0.0f))
		.VAlign(VAlign_Center)
		[
			SAssignNew(ColorWidgetBackgroundBorder, SBorder)
			.Padding(1)
			.BorderImage(FAppStyle::Get().GetBrush("ColorPicker.RoundedSolidBackground"))
			.BorderBackgroundColor(this, &FColorStructPaletteCustomization::GetColorWidgetBorderColor)
			.VAlign(VAlign_Center)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.VAlign(VAlign_Center)
				[
					SAssignNew(ColorPickerParentWidget, SColorBlock)
					.AlphaBackgroundBrush(FAppStyle::Get().GetBrush("ColorPicker.RoundedAlphaBackground"))
					.Color(this, &FColorStructPaletteCustomization::OnGetColorForColorBlock)
					.ShowBackgroundForAlpha(true)
					.AlphaDisplayMode(bIgnoreAlpha ? EColorBlockAlphaDisplayMode::Ignore : EColorBlockAlphaDisplayMode::Separate)
					.OnMouseButtonDown(this, &FColorStructPaletteCustomization::OnMouseButtonDownColorBlock)
					.Size(FVector2D(70.0f, 20.0f))
					.CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
					.IsEnabled(this, &FColorStructPaletteCustomization::IsValueEnabled, StructWeakHandlePtr)
				]
				+ SOverlay::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SBorder)
					.Visibility(this, &FColorStructPaletteCustomization::GetMultipleValuesTextVisibility)
					.BorderImage(FAppStyle::Get().GetBrush("ColorPicker.MultipleValuesBackground"))
					.VAlign(VAlign_Center)
					.ForegroundColor(FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox").ForegroundColor)
					.Padding(FMargin(12.0f, 2.0f))
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("PropertyEditor", "MultipleValues", "Multiple Values"))
						.Font(IDetailLayoutBuilder::GetDetailFont())
					]
				]
			]
		];
}

TSharedRef<SWidget> FColorStructPaletteCustomization::CreateWarningIcon()
{
	return SAssignNew(ColorWarnIcon, SImage)
		.Image(FAppStyle::GetBrush("Icons.Error"))
		.ToolTipText(INVTEXT("Warning: This color is not registered in project settings. \nCheck ColorPickerPaletteSettings \nYou can disable this warning or setup whitelist in Editor Preferences"))
		.Visibility(GetWarnIconVisibility())
		.DesiredSizeOverride(FVector2D(20.f, 20.f))
		.ColorAndOpacity(FLinearColor::Yellow);
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
				if(!this)
				{
					return;
				}
				
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

void FColorStructPaletteCustomization::OnColorPickerWindowDestroyed(const TSharedRef<SWindow>& Window)
{
	if(GWorld)
	{
		GWorld->GetTimerManager().ClearTimer(WindowDestroyTimer);
		Window.Get().GetOnWindowClosedEvent().RemoveAll(this);
		Window.Get().GetOnWindowDeactivatedEvent().RemoveAll(this);
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
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
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
	Window.Get()->GetOnWindowClosedEvent().AddRaw(this, &FColorStructPaletteCustomization::OnColorPickerWindowDestroyed);
	
	WindowContent->SetContent(CreateCustomizationWidget(Window));

	// Special case for Sequencer
	const TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if(TopLevelWindow && TopLevelWindow->GetParentWindow().IsValid() && TopLevelWindow->GetParentWindow()->GetParentWindow().IsValid())
	{
		FSlateApplication::Get().AddModalWindow(Window.ToSharedRef(), FSlateApplication::Get().GetActiveTopLevelWindow()->GetParentWindow()->GetParentWindow());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}
	
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
