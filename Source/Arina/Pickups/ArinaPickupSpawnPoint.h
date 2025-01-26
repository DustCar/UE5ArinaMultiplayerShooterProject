// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaPickupSpawnPoint.generated.h"

class AArinaPickup;

UCLASS()
class ARINA_API AArinaPickupSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AArinaPickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AArinaPickup>> PickupClasses;


	UPROPERTY()
	AArinaPickup* SpawnedPickup;

	void SpawnPickup();
	void SpawnPickupTimerFinished();
	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);

private:
	FTimerHandle SpawnPickupTimer;
	UPROPERTY(EditAnywhere)
	float SpawnTimerMin;
	UPROPERTY(EditAnywhere)
	float SpawnTimerMax;
	
public:
};
