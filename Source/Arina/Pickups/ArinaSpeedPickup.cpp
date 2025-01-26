// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaSpeedPickup.h"

#include "Arina/ArinaComponents/ArinaBuffComponent.h"
#include "Arina/Character/ArinaCharacter.h"


// Sets default values
AArinaSpeedPickup::AArinaSpeedPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArinaSpeedPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArinaSpeedPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArinaSpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	OnOverlap(ArinaCharacter);
}

void AArinaSpeedPickup::OnOverlap(AArinaCharacter* ArinaCharacter)
{
	Super::OnOverlap(ArinaCharacter);

	if (ArinaCharacter == nullptr) return;
	
	UArinaBuffComponent* ArinaBuffComponent = ArinaCharacter->GetBuffComponent();
	if (ArinaBuffComponent == nullptr) return;

	ArinaBuffComponent->BuffSpeed(SpeedBuffMult, SpeedBuffTime);

	Destroy();
}


