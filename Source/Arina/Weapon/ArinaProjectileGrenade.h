// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaProjectile.h"
#include "ArinaProjectileGrenade.generated.h"

UCLASS()
class ARINA_API AArinaProjectileGrenade : public AArinaProjectile
{
	GENERATED_BODY()

public:
	AArinaProjectileGrenade();

	void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	UPROPERTY(EditAnywhere, Category= "Projectile|SFX")
	USoundBase* BounceSound;

	
};
