// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaPickup.h"
#include "Arina/Weapon/WeaponTypes.h"
#include "ArinaAmmoPickup.generated.h"

UCLASS()
class ARINA_API AArinaAmmoPickup : public AArinaPickup
{
	GENERATED_BODY()

public:
	AArinaAmmoPickup();

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	virtual void OnOverlap(AArinaCharacter* ArinaCharacter) override;

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 0;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_MAX;
	
public:
};
