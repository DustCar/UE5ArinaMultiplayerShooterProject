// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPlayerState.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "Net/UnrealNetwork.h"

void AArinaPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Deaths);
	DOREPLIFETIME(ThisClass, KillerName);
}

void AArinaPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	ArinaCharacter = ArinaCharacter == nullptr ? Cast<AArinaCharacter>(GetPawn()) : ArinaCharacter;
	if (ArinaCharacter)
	{
		ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->Controller) : ArinaPlayerController;

		if (ArinaPlayerController)
		{
			ArinaPlayerController->SetHUDScore(GetScore());
		}
	}
}

void AArinaPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	ArinaCharacter = ArinaCharacter == nullptr ? Cast<AArinaCharacter>(GetPawn()) : ArinaCharacter;
	if (ArinaCharacter)
	{
		ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->Controller) : ArinaPlayerController;

		if (ArinaPlayerController)
		{
			ArinaPlayerController->SetHUDScore(GetScore());
		}
	}
}

void AArinaPlayerState::AddToDeaths(int32 DeathsAmount)
{
	Deaths += DeathsAmount;

	ArinaCharacter = ArinaCharacter == nullptr ? Cast<AArinaCharacter>(GetPawn()) : ArinaCharacter;
	if (ArinaCharacter)
	{
		ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->Controller) : ArinaPlayerController;

		if (ArinaPlayerController)
		{
			ArinaPlayerController->SetHUDDeaths(Deaths);
			ArinaPlayerController->DisplayKilledByMessage(KillerName);
		}
	}
}

void AArinaPlayerState::OnRep_Deaths()
{
	ArinaCharacter = ArinaCharacter == nullptr ? Cast<AArinaCharacter>(GetPawn()) : ArinaCharacter;
	if (ArinaCharacter)
	{
		ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->Controller) : ArinaPlayerController;

		if (ArinaPlayerController)
		{
			ArinaPlayerController->SetHUDDeaths(Deaths);
			ArinaPlayerController->DisplayKilledByMessage(KillerName);
		}
	}
}

void AArinaPlayerState::SetKillerName(FString Killer)
{
	KillerName = Killer;
}
