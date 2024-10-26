// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Arina/ArinaTypesHeaders/HUDPackageStruct.h"
#include "ArinaHUD.generated.h"

class UArinaCharacterOverlay;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaHUD : public AHUD
{
	GENERATED_BODY()

public:
	
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	UArinaCharacterOverlay* CharacterOverlay;

	void AddCharacterOverlay();

protected:
	virtual void BeginPlay() override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* InTexture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor InCrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& InHUDPackage) { HUDPackage = InHUDPackage; }
	
};
