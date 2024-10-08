// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaBaseWeapon.h"
#include "ArinaProjectileWeapon.generated.h"

class AArinaProjectile;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaProjectileWeapon : public AArinaBaseWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AArinaProjectile> ProjectileClass;
	
};
