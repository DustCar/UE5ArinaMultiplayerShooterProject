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

	virtual void Fire(const FVector& HitTarget) override;

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);
	void PlayImpactFX(FHitResult& FireHit);

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
	
private:
	
	/*
	*	Trace end with scatter
	*/
	UPROPERTY(EditAnywhere, Category= "WeaponScatter")
	float DistanceToSphere = 800.f;
	
	UPROPERTY(EditAnywhere, Category= "WeaponScatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category= "WeaponScatter")
	float AimedSphereRadius = 25.f;

	UPROPERTY(EditAnywhere, Category= "WeaponScatter")
	bool bUseScatter = false;
};
