// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "GameFramework/GameMode.h"
#include "ArinaGameMode.generated.h"

class AArinaPlayerController;
class AArinaCharacter;

namespace MatchState
{
	extern ARINA_API const FName Cooldown; // Match duration finished. Display winner and begin cooldown timer
}
/**
 * 
 */
UCLASS()
class ARINA_API AArinaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AArinaGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(AArinaCharacter* EliminatedCharacter, AArinaPlayerController* EliminatedController, AArinaPlayerController* AttackingController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
	
private:
	float CountdownTime = 0.f;

	int32 FindLeastOccupiedPlayerStart(TArray<AActor*>& PlayerStarts) const;
	void CalculateSmallestDistance(const TArray<AActor*>& PlayerStarts, const TArray<AActor*>& Characters, TArray<int32>& DistancesFromPlayerStarts) const;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
