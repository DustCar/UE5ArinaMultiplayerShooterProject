// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPlayerController.h"

#include "Arina/HUD/ArinaCharacterOverlay.h"
#include "Arina/HUD/ArinaHUD.h"
#include "Arina/PlayerState/ArinaPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void AArinaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArinaHUD = Cast<AArinaHUD>(GetHUD());
}

void AArinaPlayerController::SetHUDHealth(float CurrentHealth, float MaxHealth)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->HealthBar &&
		ArinaHUD->CharacterOverlay->HealthText;
	
	if (bHUDValid)
	{
		const float HealthPercent = CurrentHealth / MaxHealth;

		ArinaHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt32(CurrentHealth), FMath::CeilToInt32(MaxHealth));
		ArinaHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AArinaPlayerController::SetHUDScore(float Score)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->ScoreAmount;
	
	if (bHUDValid)
	{
		FString ScoreAmountText = FString::Printf(TEXT("%d"), FMath::TruncToInt32(Score));
		ArinaHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreAmountText));
	}
}

void AArinaPlayerController::SetHUDDeaths(int32 Deaths)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->DeathsAmount;

	if (bHUDValid)
	{
		FString DeathsAmountText = FString::Printf(TEXT("%d"), Deaths);
		ArinaHUD->CharacterOverlay->DeathsAmount->SetText(FText::FromString(DeathsAmountText));
	}
}

