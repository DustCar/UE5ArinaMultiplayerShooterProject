// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "GameFramework/GameMode.h"
#include "ArinaGameMode.generated.h"

class AArinaPlayerController;
class AArinaCharacter;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(AArinaCharacter* EliminatedCharacter, AArinaPlayerController* EliminatedController, AArinaPlayerController* AttackingController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);

protected:

private:

	int32 FindLeastOccupiedPlayerStart(TArray<AActor*>& PlayerStarts) const;
	void CalculateSmallestDistance(const TArray<AActor*>& PlayerStarts, const TArray<AActor*>& Characters, TArray<int32>& DistancesFromPlayerStarts) const;
};
