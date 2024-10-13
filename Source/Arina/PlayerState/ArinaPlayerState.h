// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ArinaPlayerState.generated.h"

class AArinaPlayerController;
class AArinaCharacter;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	*	Replication notifies
	*/
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Deaths();
	
	void AddToScore(float ScoreAmount);
	void AddToDeaths(int32 DeathsAmount);

private:
	UPROPERTY()
	AArinaCharacter* ArinaCharacter;

	UPROPERTY()
	AArinaPlayerController* ArinaPlayerController;

	UPROPERTY(ReplicatedUsing=OnRep_Deaths)
	int32 Deaths;
	
};
