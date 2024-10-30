// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ArinaGameState.generated.h"

class AArinaPlayerState;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateLeaderboard(AArinaPlayerState* ScoringPlayer);
	
	UPROPERTY(Replicated)
	TArray<AArinaPlayerState*> PlayersLeaderboard;

private:
	float TopScore = 0.f;
};
