// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaRocketMovementComponent.h"


UArinaRocketMovementComponent::UArinaRocketMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UArinaRocketMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UArinaRocketMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
												  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

UProjectileMovementComponent::EHandleBlockingHitResult UArinaRocketMovementComponent::HandleBlockingHit(
	const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);

	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void UArinaRocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// Rocket should not stop; Only explode when their collision box detects a hit
}

