// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPickupSpawnPoint.h"

#include "ArinaPickup.h"


AArinaPickupSpawnPoint::AArinaPickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AArinaPickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	SpawnPickupTimerFinished();
}

void AArinaPickupSpawnPoint::SpawnPickup()
{
	int32 NumPickupClasses = PickupClasses.Num();
	if (NumPickupClasses == 0) return;

	int32 SelectedPickupClass = FMath::RandRange(0, NumPickupClasses - 1);
	SpawnedPickup = GetWorld()->SpawnActor<AArinaPickup>(PickupClasses[SelectedPickupClass], GetActorTransform());

	if (HasAuthority() && SpawnedPickup)
	{
		SpawnedPickup->OnDestroyed.AddDynamic(this, &ThisClass::StartSpawnPickupTimer);
	}
}

void AArinaPickupSpawnPoint::SpawnPickupTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void AArinaPickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnTimerMin, SpawnTimerMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickupTimer,
		this,
		&ThisClass::SpawnPickupTimerFinished,
		SpawnTime
	);
}

void AArinaPickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

