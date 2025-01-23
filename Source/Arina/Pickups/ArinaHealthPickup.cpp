// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaHealthPickup.h"

#include "Arina/ArinaComponents/ArinaBuffComponent.h"
#include "Arina/Character/ArinaCharacter.h"


AArinaHealthPickup::AArinaHealthPickup()
{
	bReplicates = true;
	
}

void AArinaHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	if (ArinaCharacter == nullptr) return;
	
	UArinaBuffComponent* ArinaBuffComponent = ArinaCharacter->GetBuffComponent();
	if (ArinaBuffComponent == nullptr || ArinaCharacter->GetHealth() == ArinaCharacter->GetMaxHealth()) return;

	ArinaBuffComponent->Heal(HealAmount, HealingTime);

	Destroy();
}


