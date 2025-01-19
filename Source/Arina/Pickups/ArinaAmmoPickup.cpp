// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAmmoPickup.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"

AArinaAmmoPickup::AArinaAmmoPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
}

void AArinaAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	if (ArinaCharacter && ArinaCharacter->GetCombatComponent() && ArinaCharacter->GetEquippedWeapon())
	{
		ArinaCharacter->GetCombatComponent()->PickupAmmo(WeaponType, AmmoAmount);

		Destroy();
	}

}


