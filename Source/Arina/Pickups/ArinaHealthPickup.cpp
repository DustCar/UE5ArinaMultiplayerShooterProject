// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaHealthPickup.h"

#include "Arina/ArinaComponents/ArinaBuffComponent.h"
#include "Arina/Character/ArinaCharacter.h"


AArinaHealthPickup::AArinaHealthPickup()
{
	
}

void AArinaHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	OnOverlap(ArinaCharacter);
}

// same functionality as original OnSphereOverlap but alse used for when a player is directly on top of a pickup being spawned
void AArinaHealthPickup::OnOverlap(AArinaCharacter* ArinaCharacter)
{
	Super::OnOverlap(ArinaCharacter);

	if (ArinaCharacter == nullptr) return;
	
	UArinaBuffComponent* ArinaBuffComponent = ArinaCharacter->GetBuffComponent();
	if (ArinaBuffComponent == nullptr || ArinaCharacter->GetHealth() == ArinaCharacter->GetMaxHealth()) return;

	ArinaBuffComponent->Heal(HealAmount, HealingTime, ArinaCharacter->GetHealth());

	Destroy();
}


