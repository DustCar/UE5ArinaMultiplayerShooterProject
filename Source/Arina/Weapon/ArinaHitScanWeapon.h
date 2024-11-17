// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaBaseWeapon.h"
#include "ArinaHitScanWeapon.generated.h"

UCLASS()
class ARINA_API AArinaHitScanWeapon : public AArinaBaseWeapon
{
	GENERATED_BODY()

public:
	AArinaHitScanWeapon();
	virtual void Tick(float DeltaTime) override;

	virtual void Fire(const FVector& HitTarget) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	float BulletDamage = 10.f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactFX;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BodyHitFX;

	UPROPERTY(EditAnywhere)
	USoundBase* BodyHitSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamFX;
};
