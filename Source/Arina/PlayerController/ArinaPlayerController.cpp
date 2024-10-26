// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPlayerController.h"

#include "Arina/HUD/ArinaCharacterOverlay.h"
#include "Arina/HUD/ArinaHUD.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Arina/GameMode/ArinaGameMode.h"
#include "Net/UnrealNetwork.h"

void AArinaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArinaHUD = Cast<AArinaHUD>(GetHUD());
}

void AArinaPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
}

void AArinaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ClientCollapseKilledByMessage();
}

void AArinaPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();

	CheckTimeSync(DeltaSeconds);
}

void AArinaPlayerController::CheckTimeSync(float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AArinaPlayerController::SetHUDHealth(const float& CurrentHealth, const float& MaxHealth)
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

void AArinaPlayerController::SetHUDScore(const float& Score)
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

void AArinaPlayerController::SetHUDDeaths(const int32& Deaths)
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

void AArinaPlayerController::SetHUDWeaponAmmo(const int32& WeaponAmmo)
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

void AArinaPlayerController::SetHUDCarryAmmo(const int32& CarryAmmo)
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

void AArinaPlayerController::SetHUDWeaponType(const FString& WeaponType)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->WeaponType;

	if (bHUDValid)
	{
		FString WeaponText = FString::Printf(TEXT("%s"), *WeaponType);
		ArinaHUD->CharacterOverlay->WeaponType->SetText(FText::FromString(WeaponText));
	}
}

void AArinaPlayerController::DisplayKilledByMessage(const FString& KillerName)
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
void AArinaPlayerController::CollapseKilledByMessage()
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

void AArinaPlayerController::ClientCollapseKilledByMessage_Implementation()
{
	CollapseKilledByMessage();
}

void AArinaPlayerController::SetHUDMatchTimer(const float CountdownTime)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->CharacterOverlay &&
		ArinaHUD->CharacterOverlay->MatchTimerText;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt32(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ArinaHUD->CharacterOverlay->MatchTimerText->SetText(FText::FromString(CountdownText));
	}
}

void AArinaPlayerController::SetHUDTime()
{
	float SecondsLeft = MatchTime - GetServerTime();

	if (CountDownInt != FMath::CeilToInt32(SecondsLeft))
	{
		SetHUDMatchTimer(SecondsLeft);
	}

	CountDownInt = FMath::CeilToInt32(SecondsLeft);

}

void AArinaPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AArinaPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedRequest + 0.5f * RoundTripTime;

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AArinaPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void AArinaPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AArinaPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;
		if (ArinaHUD)
		{
			ArinaHUD->AddCharacterOverlay();
			SetHUDScore(0.f);
			SetHUDDeaths(0);
			CollapseKilledByMessage();
			SetHUDWeaponType("UnEquipped");
		}
	}
}

void AArinaPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;
		if (ArinaHUD)
		{
			ArinaHUD->AddCharacterOverlay();
			SetHUDScore(0.f);
			SetHUDDeaths(0);
			CollapseKilledByMessage();
			SetHUDWeaponType("UnEquipped");
		}
	}
}
