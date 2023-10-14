// Copyright 2023 Yvidge. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ColorPickerPaletteUserData.generated.h"

USTRUCT(BlueprintType)
struct FColorPickerPaletteUserData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	bool bCollapsed = false;
};
