// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPlayerController.h"

#include "Arina/HUD/ArinaCharacterOverlay.h"
#include "Arina/HUD/ArinaHUD.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void AArinaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArinaHUD = Cast<AArinaHUD>(GetHUD());
}

void AArinaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MulticastCollapseKilledByMessage();
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

void AArinaPlayerController::SetHUDWeaponAmmo(int32 WeaponAmmo)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), WeaponAmmo);
		ArinaHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AArinaPlayerController::SetHUDCarryAmmo(int32 CarryAmmo)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->CarryAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), CarryAmmo);
		ArinaHUD->CharacterOverlay->CarryAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AArinaPlayerController::DisplayKilledByMessage(FString KillerName)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->KilledByBox &&
		ArinaHUD->CharacterOverlay->KilledByName;

	if (bHUDValid)
	{
		ArinaHUD->CharacterOverlay->KilledByName->SetText(FText::FromString(KillerName));
		ArinaHUD->CharacterOverlay->KilledByBox->SetVisibility(ESlateVisibility::Visible);
	}
}
void AArinaPlayerController::MulticastCollapseKilledByMessage_Implementation()
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->KilledByBox &&
		ArinaHUD->CharacterOverlay->KilledByName;

	if (bHUDValid)
	{
		ArinaHUD->CharacterOverlay->KilledByBox->SetVisibility(ESlateVisibility::Collapsed);
		ArinaHUD->CharacterOverlay->KilledByName->SetText(FText::FromString(""));
	}
}



