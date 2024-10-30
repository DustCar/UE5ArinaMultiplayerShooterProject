#pragma once

#include "HUDPackageStruct.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UTexture2D* CrosshairCenter;
	UPROPERTY()
	UTexture2D* CrosshairRight;
	UPROPERTY()
	UTexture2D* CrosshairLeft;
	UPROPERTY()
	UTexture2D* CrosshairTop;
	UPROPERTY()
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
};