// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaShieldPickup.h"

#include "Arina/ArinaComponents/ArinaBuffComponent.h"
#include "Arina/Character/ArinaCharacter.h"


AArinaShieldPickup::AArinaShieldPickup()
{
}

void AArinaShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	OnOverlap(ArinaCharacter);
}

void AArinaShieldPickup::OnOverlap(AArinaCharacter* ArinaCharacter)
{
	Super::OnOverlap(ArinaCharacter);

	if (ArinaCharacter == nullptr) return;
	
	UArinaBuffComponent* ArinaBuffComponent = ArinaCharacter->GetBuffComponent();
	if (ArinaBuffComponent == nullptr || ArinaCharacter->GetShield() == ArinaCharacter->GetMaxShield()) return;

	ArinaBuffComponent->ReplenishShield(ShieldReplenishAmount);

	Destroy();
}


