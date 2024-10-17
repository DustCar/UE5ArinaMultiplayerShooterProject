// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArinaCharacterOverlay.generated.h"

class UHorizontalBox;
class UTextBlock;
class UProgressBar;
/**
 *	Widget for all information that the player probably must see, like health, ammo, game stats, etc.
 */
UCLASS()
class ARINA_API UArinaCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsAmount;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* KilledByBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledByText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledByName;
	
};
