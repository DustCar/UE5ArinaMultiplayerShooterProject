// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaGameMode.h"

#include "Arina/Character/ArinaCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AArinaGameMode::PlayerEliminated(AArinaCharacter* EliminatedCharacter,
                                      AArinaPlayerController* EliminatedController, AArinaPlayerController* AttackingController)
{
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Eliminated();
	}
}

void AArinaGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Reset();
		EliminatedCharacter->Destroy();
	}

	if (EliminatedController)
	{
		TArray<AActor*> PlayerStarts;
		int32 Selected = FindLeastOccupiedPlayerStart(PlayerStarts);
		if (Selected >= 0)
		{
			RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selected]);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FindLeastOccupiedPlayerStart() function failed to calculate!"))
		}
	}
}

// Calculate and saves the closest distance to a character for each player start
void AArinaGameMode::CalculateSmallestDistance(const TArray<AActor*>& PlayerStarts, const TArray<AActor*>& Characters, TArray<int32>& DistancesFromPlayerStarts) const
{
	
	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		int32 MinDistance = MAX_int32;
		for (int32 j = 0; j < Characters.Num(); j++)
		{
			const AActor* CurrentPlayerStart = PlayerStarts[i];
			const AActor* CurrentCharacter = Characters[j];
			int32 Distance = (CurrentPlayerStart->GetActorLocation() - CurrentCharacter->GetActorLocation()).Size();

			if (Distance < MinDistance)
			{
				MinDistance = Distance;
			}
		}

		DistancesFromPlayerStarts[i] = MinDistance;
	}
}

int32 AArinaGameMode::FindLeastOccupiedPlayerStart(TArray<AActor*>& PlayerStarts) const
{
	int32 BestPlayerStart = -1;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(this, AArinaCharacter::StaticClass(), Characters);

	TArray<int32> DistancesFromPlayerStarts;
	DistancesFromPlayerStarts.Init(0, PlayerStarts.Num());
	
	CalculateSmallestDistance(PlayerStarts, Characters, DistancesFromPlayerStarts);
	
	// Choose the player start with the farthest distance from their closest player
	int32 MaxDistance = 0;
	for (int32 i = 0; i < DistancesFromPlayerStarts.Num(); i++)
	{
		if (DistancesFromPlayerStarts[i] > MaxDistance)
		{
			BestPlayerStart = i;
			MaxDistance = DistancesFromPlayerStarts[i];
		}
	}

	return BestPlayerStart;
}
