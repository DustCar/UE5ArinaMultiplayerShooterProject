// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPlayerController.h"

#include "Arina/HUD/ArinaCharacterOverlay.h"
#include "Arina/HUD/ArinaHUD.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Arina/GameMode/ArinaGameMode.h"
#include "Arina/HUD/ArinaAnnouncement.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AArinaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ArinaHUD = Cast<AArinaHUD>(GetHUD());
	
	ServerCheckMatchState();
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

void AArinaPlayerController::ServerCheckMatchState_Implementation()
{
	AArinaGameMode* GameMode = Cast<AArinaGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		CooldownTime = GameMode->CooldownTime;
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);
	}
}

void AArinaPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float CDTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	CooldownTime = CDTime;
	OnMatchStateSet(MatchState);

	if (ArinaHUD && MatchState == MatchState::WaitingToStart)
	{
		ArinaHUD->AddAnnouncement();
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

void AArinaPlayerController::SetHUDAnnouncementTimer(const float CountdownTime)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->Announcement &&
		ArinaHUD->Announcement->PreMatchTime;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt32(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ArinaHUD->Announcement->PreMatchTime->SetText(FText::FromString(CountdownText));
	}
}

void AArinaPlayerController::SetHUDCooldownTimer(const float CountdownTime)
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;

	bool bHUDValid = ArinaHUD &&
		ArinaHUD->Announcement &&
		ArinaHUD->Announcement->PreMatchTime &&
		ArinaHUD->Announcement->PostMatchTime &&
		ArinaHUD->Announcement->AnnouncementText;

	if (bHUDValid)
	{
		FString AnnounceText = FString(TEXT("New Match Starts In: "));
		int32 Minutes = FMath::FloorToInt32(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ArinaHUD->Announcement->PreMatchTime->SetText(FText::FromString(""));
		ArinaHUD->Announcement->PostMatchTime->SetText(FText::FromString(CountdownText));
		ArinaHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnounceText));
	}
}

void AArinaPlayerController::SetHUDTime()
{
	// Make sure to capture LevelStartingTime when game mode is valid on the server before using in TimeLeft
	if (HasAuthority())
	{
		AArinaGameMode* ArinaGameMode = Cast<AArinaGameMode>(UGameplayStatics::GetGameMode(this));
		{
			if (ArinaGameMode)
			{
				LevelStartingTime = ArinaGameMode->LevelStartingTime;
			}
		}
	}
	
	float TimeLeft = 0.f;
	// When Player joins since session is created
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	// When Player joins mid-game. Adds WarmupTime since session would have passed it.
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = WarmupTime + MatchTime + CooldownTime - GetServerTime() + LevelStartingTime;
	}
	
	uint32 SecondsLeft = FMath::CeilToInt32(TimeLeft);
	if (CountDownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementTimer(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchTimer(TimeLeft);
		}
		if (MatchState == MatchState::Cooldown)
		{
			SetHUDCooldownTimer(TimeLeft);
		}
	}
	CountDownInt = SecondsLeft;
}

void AArinaPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AArinaPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedRequest)
{
	// Time it takes for a request to go from the client to server then back to client.
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	// Half of RoundTripTime roughly estimates to the time the server takes to send the request back to client.
	float CurrentServerTime = TimeServerReceivedRequest + 0.5f * RoundTripTime;
	
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AArinaPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
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
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AArinaPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AArinaPlayerController::HandleMatchHasStarted()
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;
	if (ArinaHUD)
	{
		if (ArinaHUD->Announcement)
		{
			ArinaHUD->Announcement->SetVisibility(ESlateVisibility::Collapsed);
		}
		ArinaHUD->AddCharacterOverlay();
		SetHUDScore(0.f);
		SetHUDDeaths(0);
		CollapseKilledByMessage();
		SetHUDWeaponType("UnEquipped");
	}
}

void AArinaPlayerController::HandleCooldown()
{
	ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(GetHUD()) : ArinaHUD;
	if (ArinaHUD)
	{
		ArinaHUD->CharacterOverlay->RemoveFromParent();
		if (ArinaHUD->Announcement)
		{
			ArinaHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
