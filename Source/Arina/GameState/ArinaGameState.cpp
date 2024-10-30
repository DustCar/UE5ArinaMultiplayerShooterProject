// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaGameState.h"

#include "Arina/PlayerState/ArinaPlayerState.h"
#include "Net/UnrealNetwork.h"

void AArinaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayersLeaderboard);
}

void AArinaGameState::UpdateLeaderboard(AArinaPlayerState* ScoringPlayer)
{
	PlayersLeaderboard.AddUnique(ScoringPlayer);
	if (PlayersLeaderboard.Num() > 1)
	{
		PlayersLeaderboard.Sort(
	[] (const AArinaPlayerState& A, const AArinaPlayerState& B)
			{
				return A.GetScore() > B.GetScore();
			}
		);
	}
}
