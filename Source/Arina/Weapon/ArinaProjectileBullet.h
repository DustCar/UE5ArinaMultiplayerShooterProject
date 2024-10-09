// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaProjectile.h"
#include "ArinaProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class ARINA_API AArinaProjectileBullet : public AArinaProjectile
{
	GENERATED_BODY()

public:

protected:

	virtual void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit) override;

private:
	
};
