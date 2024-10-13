// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaHUD.h"

#include "ArinaCharacterOverlay.h"
#include "Arina/PlayerController/ArinaPlayerController.h"

void AArinaHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize = FVector2D::ZeroVector;

	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		
		if (HUDPackage.CrosshairCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, FVector2D(0.f,0.f), HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairRight)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairLeft)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairTop)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairBottom)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
	}
}

void AArinaHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AArinaHUD::AddCharacterOverlay()
{
	APlayerController* PC = GetOwningPlayerController();
	if (PC && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UArinaCharacterOverlay>(PC, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();

		AArinaPlayerController* ArinaPlayerController = Cast<AArinaPlayerController>(PC);
		if (ArinaPlayerController)
		{
			ArinaPlayerController->SetHUDScore(0.f);
			ArinaPlayerController->SetHUDDeaths(0);
		}
	}
}

void AArinaHUD::DrawCrosshair(UTexture2D* InTexture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor InCrosshairColor)
{
	const float TextureWidth = InTexture->GetSizeX();
	const float TextureHeight = InTexture->GetSizeY();

	// set to use the center of the texture rather than the top left (default)
	const FVector2D TextureDrawPoint = ViewportCenter - FVector2D((TextureWidth / 2.f) + Spread.X, (TextureHeight / 2.f) + Spread.Y);

	DrawTexture(
		InTexture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		InCrosshairColor
		);
}
